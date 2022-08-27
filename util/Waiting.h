// Copyright 2019 The MWC Developers
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef MWC_QT_WALLET_WAITING_H
#define MWC_QT_WALLET_WAITING_H

namespace util {

// When class created, it sswitch to waiting cursor.
// At destructor - restore the cursor back
// Note: usage expected at automatic object only
class Waiting {
public:
    Waiting();
   ~Waiting();
};

}

#endif //MWC_QT_WALLET_WAITING_H