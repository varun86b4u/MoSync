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

#include <ma.h>
#include <MAUtil/String.h>
#include <MAUtil/Moblet.h>
#include <conprint.h>
#include <maapi.h>

using namespace MAUtil;

#include "webImageDownloader.h"
/*
 * 8 is the limit of consecutive downloads for Symbian,
 * other platforms can do better.
 * 32 is the limit on iOS.
 */
#define NUM_DOWNLOADS 8
#define NUM_BATCHES 10

/**
 * Moblet for the  application.
 */
class myMoblet : public Moblet, public webImageDownloaderListener {
public:
	myMoblet()
	{
		mBatchNumber = 0;

		printf("Number of parallel downloads:%d, number of batches:%d", NUM_DOWNLOADS, NUM_BATCHES);
		startDownloads();


	}

	void startDownloads()
	{
		mFinishedDownloaders = 0;
		MAUtil::String url = "http://www.mosync.com/files/images/mosync-color-horizontal-dark.png";
		mBatchNumber++;
		printf("STARTING DOWNLOAD BATCH %d", mBatchNumber);
		for(int i = 0; i < NUM_DOWNLOADS; i++)
		{
			webImageDownloader * imagedownload = new webImageDownloader(url, "newdataname1", this);
		}
	}

	void downloadFinished(webImageDownloader *downloader)
	{
		delete downloader;
		mFinishedDownloaders++;

		if(mFinishedDownloaders == NUM_DOWNLOADS)
		{
			if(mBatchNumber < NUM_BATCHES)
			{
				startDownloads();
			}
			else
			{
				printf("TEST PASSED");
			}
		}
	}

	void myMoblet::init() {}
	void myMoblet::draw() {}

	/**
	 * Destructor.
	 */
	virtual ~myMoblet(){}

private:
	int mFinishedDownloaders;
	int mBatchNumber;
};

extern "C" int MAMain()
{
	InitConsole();
	gConsoleLogging = 1;

	// Run the moblet event loop.
	//Moblet::run(moblet);
	Moblet::run(new myMoblet());

	// Deallocate objects.
	//delete Moblet;
	return 0;
}
