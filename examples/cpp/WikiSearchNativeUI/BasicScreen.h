/* Copyright 2013 David Axmark

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/**
 * @file BasicScreen.h
 * @author Emma Tresanszki
 *
 * This file contains the base screen of the application.
 * It contains the base screen constructors, and the creation
 * of a main layout that is common for some screens.
 */

#ifndef BASICSCREENNUI_H_
#define BASICSCREENNUI_H_

// Include MoSync syscalls collection.
#include <IX_WIDGET.h>

namespace WikiNativeUI
{

/**
 * BasicScreen native UI.
 * When using many screens, inherit from this basic screen class.
 * which creates a screen with a main vertical layout, that fits the screen size.
 */
class BasicScreen
{
public:

	/**
	 * Constructor for the basic screen class.
	 * Here instance variables are initialized.
	 */
	BasicScreen();

	/**
	 * Destructor.
	 * Delete the main widget, and all it's children will be deleted also.
	 * The base class destructor will be called after the derived class
	 * destructor is called.
	 */
	virtual ~BasicScreen();

	/**
	 * Sets the font size and the padding size depending on the screen size.
	 */
	void setSizes();

	/**
	 * Show the screen.
	 */
	virtual void showScreen();

	// **************** Getters  ****************

	/**
	 * Return a handle to the screen widget.
	 * @return Handle to the screen widget.
	 */
	MAWidgetHandle getScreen() const;

	/**
	 * Return a handle to the main layout.
	 * @return Handle to the main layout.
	 */
	MAWidgetHandle getMainLayout();

	/**
	 * Return a handle to the top label.
	 * @return Handle to the top label.
	 */
	MAWidgetHandle getTopLabel();

	/**
	 * Return a handle to one of the top buttons (Back).
	 * @return handle to the top button.
	 */
	MAWidgetHandle getTopButtonLeft();

	/**
	 * Return a handle to one of the top buttons (Next).
	 * @return handle to the top button.
	 */
	MAWidgetHandle getTopButtonRight();

private: // methods
	/**
	 * Create the main layout, to fit the screen.
	 * @return Handle to the main layout.
	 */
	MAWidgetHandle createBaseMainLayout();

	/**
	 * Utility function to create a layout that is used in all
	 * screens, and it is contained in the main layout.
	 * It is a top layout that contains Back and Next buttons.
	 * The default text for the buttons is null,
	 * and can be later set.
	 * @return Access to the handle of the created layout.
	 */
	MAWidgetHandle createTopLayout();

	/**
	 * Lay out the widgets (portrait mode).
	 * These widgets are common for most screens.
	 */
	void setupUI();

protected:
	// protected keyword: these variables will be automatically
	// inherited by all the derived class but not outside the
	// base and derived class of BasicScreenNUI.

	/** Screen's width. */
	int mScreenWidth;

	/** Screen's height. */
	int mScreenHeight;

	/** The font size. */
	int mFontSize;

	/** The padding size. */
	int mPaddingSize;

private: // members
	/** The screen handle. */
	MAWidgetHandle mScreen;

	/** The main layout of the screen. */
	MAWidgetHandle mMainLayout;

	MAWidgetHandle mTopLabel;

	/** The  next button of the top layout. */
	MAWidgetHandle mTopButtonPrevious;

	/** The  previous button of the top layout. */
	MAWidgetHandle mTopButtonNext;

};

} // namespace WikiNativeUI

#endif /* BASICSCREENNUI_H_ */
