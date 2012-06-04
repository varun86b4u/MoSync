/*
 Copyright (C) 2012 MoSync AB

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License,
 version 2, as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 MA 02110-1301, USA.
 */

/**
 * @file PurchaseManager.cpp
 * @author Bogdan Iusco
 * @date 3 May 2012
 *
 * @brief The PurchaseManager manages purchase related events and dispatches
 * them to the target products.
 */

#include "PurchaseManager.h"
#include "PurchaseManagerListener.h"
#include "Purchase.h"

namespace IAP
{
	/**
	 * Initialize the singleton variable to NULL.
	 */
	PurchaseManager* PurchaseManager::sInstance = NULL;

	/**
	*  Check if in app purchase is supported/enabled on a device.
	* @return One of the next result codes:
	* - MA_PURCHASE_RES_OK if purchase is supported/allowed on the device.
	* - MA_PURCHASE_RES_DISABLED if purchase is not allowed/enabled.
	*/
	int PurchaseManager::checkPurchaseSupported()
	{
		return maPurchaseSupported();
	}

	/**
	 * Constructor.
	 */
	PurchaseManager::PurchaseManager()
	{
		// Add me as a custom event listener.
		MAUtil::Environment::getEnvironment().addCustomEventListener(this);
	}

	/**
	 * Destructor.
	 */
	PurchaseManager::~PurchaseManager()
	{
		// Remove me as a custom event listener.
		MAUtil::Environment::getEnvironment().removeCustomEventListener(this);
		mListeners.clear();
		mPurchaseMap.clear();
		mSpecialProducts.clear();
	}

	/**
	 * Copy constructor.
	 */
	PurchaseManager::PurchaseManager(const PurchaseManager& purchaseManager):
		MAUtil::CustomEventListener()
	{
	}

	/**
	 * Return the single instance of this class.
	 */
	PurchaseManager* PurchaseManager::getInstance()
	{
		if (PurchaseManager::sInstance == NULL)
		{
			PurchaseManager::sInstance = new PurchaseManager();
		}

		return sInstance;
	}

	/**
	 * Destroy the single instance of this class.
	 * Call this method only when the application will exit.
	 */
	void PurchaseManager::destroyInstance()
	{
		delete PurchaseManager::sInstance;
	}

	/**
	 * Set your Google Play public key to the application. This enables the
	 * application to verify the signature of the transaction information
	 * that is returned from Google Play.
	 * This method is mandatory for being able to request for purchases.
	 * Platform: Android.
	 * @param developerPublicKey Base64-encoded public key, that can be found
	 * on the Google Play publisher account page, under Licensing & In-app
	 * Billing panel in Edit Profile.
	 */
	void PurchaseManager::setPublicKey(const MAUtil::String& developerPublicKey)
	{
		maPurchaseSetPublicKey(developerPublicKey.c_str());
	}

	/**
	 * Set the store URL used for verifying the receipt on iOS platform.
	 * Platform: iOS.
	 * @param url Recommended values: sAppStoreURL and sAppStoreSandboxURL.
	 */
	void PurchaseManager::setStoreURL(const MAUtil::String& url)
	{
		maPurchaseSetStoreURL(url.c_str());
	}

	/**
	 * Restore transactions that were previously finished so that you can process
	 * them again. For example, your application would use this to allow a user to
	 * unlock previously purchased content onto a new device.
	 * Listeners will be notified when a purchase is restored, or in the case of an error.
	 */
	void PurchaseManager::restoreTransactions()
	{
		maPurchaseRestoreTransactions();
	}

	/**
	 * Implementation of CustomEventListener interface.
	 * This method will get called whenever there is a
	 * widget event generated by the system.
	 * @param event The new received event.
	 */
	void PurchaseManager::customEvent(const MAEvent& event)
	{
		if (event.type != EVENT_TYPE_PURCHASE)
		{
			return;
		}

		MAPurchaseEventData purchaseData = event.purchaseData;
		switch (purchaseData.type )
		{
			case MA_PURCHASE_EVENT_RESTORE:
				this->handleProductRestoreEvent(purchaseData);
				break;
			case MA_PURCHASE_EVENT_REFUNDED:
				this->createRefundedProduct(purchaseData.productHandle);
				break;
			default:
				this->notifyListener(purchaseData);
				break;
		}
	}

	/**
	 * Add a purchase to the map that holds purchases.
	 * The purchase will receive custom events.
	 * @param purchase The purchase that needs to be registered.
	 * The ownership of the purchase is not passed to this method.
	 */
	void PurchaseManager::registerPurchase(Purchase* purchase)
	{
		mPurchaseMap[purchase->getHandle()] = purchase;
	}

	/**
	 * Remove a purchase from the map that holds purchases.
	 * The purchase will not receive custom events.
	 * @param product The purchase that needs to be unregistered.
	 */
	void PurchaseManager::unregisterPurchase(Purchase* purchase)
	{
		mPurchaseMap.erase(purchase->getHandle());
	}

	/**
	 * Add an event listener for purchase.
	 * Listener will be notified about restored or refunded products.
	 * @param listener The listener that will receive notifications.
	 */
	void PurchaseManager::addPurchaseListener(PurchaseManagerListener* listener)
	{
		int countListeners = mListeners.size();
		for (int i = 0; i < countListeners; i++)
		{
			if (listener == mListeners[i])
			{
				return;
			}
		}

		mListeners.add(listener);

	}

	/**
	 * Remove the event listener for purchase.
	 * @param listener The listener that receives purchase notifications.
	 */
	void PurchaseManager::removePurchaseListener(PurchaseManagerListener* listener)
	{
		int countListeners = mListeners.size();
		for (int i = 0; i < countListeners; i++)
		{
			if (listener == mListeners[i])
			{
				mListeners.remove(i);
				break;
			}
		}
	}

	/**
	 * Create a product that has been restored and notifies the listeners.
	 * @param productHandle Handle to the product that has been restored.
	 */
	void PurchaseManager::createRestoredProduct(MAHandle productHandle)
	{
		Purchase* purchase = new Purchase(productHandle);
		this->unregisterPurchase(purchase);
		int countListeners = mListeners.size();
		for (int i = 0; i < countListeners; i++)
		{
			mListeners[i]->purchaseRestored(*purchase);
		}
		mSpecialProducts.add(purchase);
	}

	/**
	 * Create a product that has been refunded and notifies the listeners.
	 * @param productHandle Handle to the product that has been refunded.
	 */
	void PurchaseManager::createRefundedProduct(MAHandle productHandle)
	{
		Purchase* purchase = new Purchase(productHandle);
		this->unregisterPurchase(purchase);
		int countListeners = mListeners.size();
		for (int i = 0; i < countListeners; i++)
		{
			mListeners[i]->purchaseRefunded(*purchase);
		}
		mSpecialProducts.add(purchase);
	}

	/**
	 * Handle a MA_PURCHASE_EVENT_RESTORE event.
	 * Notifies listeners about the event.
	 * @param purchaseData Event data.
	 */
	void PurchaseManager::handleProductRestoreEvent(
		const MAPurchaseEventData& purchaseData)
	{
		if (purchaseData.state == MA_PURCHASE_STATE_RESTORE_ERROR)
		{
			int countListeners = mListeners.size();
			for (int i = 0; i < countListeners; i++)
			{
				mListeners[i]->purchaseRestoreError(purchaseData.errorCode);
			}
		}
		else
		{
			this->createRestoredProduct(purchaseData.productHandle);
		}
	}

	/**
	 * Forwards an event to its purchase object.
	 * @param purchaseData purchaseData Event data.
	 */
	void PurchaseManager::notifyListener(
		const MAPurchaseEventData& purchaseData)
	{
		// Check if the purchase exists in the map.
		if (mPurchaseMap.end() != mPurchaseMap.find(purchaseData.productHandle))
		{
			// Get the product object that wraps the product handle.
			Purchase* purchase = mPurchaseMap[purchaseData.productHandle];
			// Call the purchase's event handling method.
			purchase->handlePurchaseEvent(purchaseData);
			return;
		}
		// Check if the purchase exists in mSpecialProducts, and handle receipt
		// events for refunded or restored products.
		if ( purchaseData.type == MA_PURCHASE_EVENT_VERIFY_RECEIPT )
		{
			for (int i=0; i < mSpecialProducts.size(); i++)
			{
				if ( mSpecialProducts[i]->getHandle() == purchaseData.productHandle )
				{
					mSpecialProducts[i]->handlePurchaseEvent(event.purchaseData);
					break;
				}
			}
		}
	}
}
