/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "stack.h"

StackItem::StackItem()
	{}

StackItem::~StackItem()
	{}

Stack::Stack()
	{}
	
StackItem * Stack::Pop()
	{
	StackItem* item=(StackItem*)LinkedList::TailItem();
	LinkedList::RemoveItem(item);
	return item;
	}

void Stack::Push(StackItem* aNewItem)
	{
	LinkedList::AddToTail(aNewItem);
	}
	
StackItem * Stack::Peek()
	{
	return (StackItem*)LinkedList::TailItem();
	}
