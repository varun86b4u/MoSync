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

template<class T> class FlagCheck {
private:
	int flags;
public:
	FlagCheck() : flags(0) {}

	// set a flag.
	void set(typename T::Flags f) {
		flags |= 1 << f;
	}

	// returns true if all flags are set.
	bool check() {
		return flags == ((1 << T::_flags_end) - 1);
	}
};
