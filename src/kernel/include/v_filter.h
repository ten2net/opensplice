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
#ifndef V_FILTER_H
#define V_FILTER_H

#include "v_kernel.h"

#if defined (__cplusplus)
extern "C" {
#endif

#ifdef OSPL_BUILD_CORE
#define OS_API OS_API_EXPORT
#else
#define OS_API OS_API_IMPORT
#endif

/* !!!!!!!!NOTE From here no more includes are allowed!!!!!!! */
#define v_filter(f) (C_CAST(f,v_filter))

OS_API v_filter
v_filterNew (
    v_topic t,
    q_expr e,
    const c_value params[]);

OS_API v_filter
v_filterNewFromIndex (
    v_index t,
    q_expr e,
    const c_value params[]);

OS_API c_bool
v_filterEval (
    v_filter _this,
    c_object o);

OS_API void
v_filterSplit (
    v_topic topic,
    q_expr where,
    const c_value params[],
    c_array *instanceQ,
    c_array *sampleQ,
    v_index index);

#undef OS_API

#if defined (__cplusplus)
}
#endif

#endif /* V_FILTER_H */

