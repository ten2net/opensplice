/*
 *                         OpenSplice DDS
 *
 *   This software and documentation are Copyright 2006 to TO_YEAR PrismTech
 *   Limited, its affiliated companies and licensors. All rights reserved.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */
#ifndef CCPP_ORB_ABSTRACTION_H
#define CCPP_ORB_ABSTRACTION_H

#include <tao/LocalObject.h>
#include <tao/Valuetype/ValueBase.h>
#include <tao/Version.h>

  namespace DDS
  {
// Sometime post TAO 1.6.6 the deprecated TAO_Local_RefCounted_Object has been removed.
// CORBA::LocalObject has been ref counted & fine to use instead since 1.6.5 at least.
#if (TAO_MAJOR_VERSION == 1 && TAO_MINOR_VERSION == 6 && TAO_BETA_VERSION > 4) || TAO_MINOR_VERSION > 6 || TAO_MAJOR_VERSION > 1
    #define LOCAL_REFCOUNTED_OBJECT ::CORBA::LocalObject
#else
    #define LOCAL_REFCOUNTED_OBJECT ::TAO_Local_RefCounted_Object
#endif
    #define LOCAL_REFCOUNTED_VALUEBASE ::CORBA::DefaultValueRefCountBase

    #define THROW_ORB_EXCEPTIONS
    #define THROW_ORB_AND_USER_EXCEPTIONS(...)

    #define THROW_VALUETYPE_ORB_EXCEPTIONS
    #define THROW_VALUETYPE_ORB_AND_USER_EXCEPTIONS(...)
  };

#endif /* ORB_ABSTRACTION */
