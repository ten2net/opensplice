#include "saj_condition.h"
#include "saj_utilities.h"

#define SAJ_FUNCTION(name) Java_org_opensplice_dds_dcps_ConditionImpl_##name

/**
 * Class:     org_opensplice_dds_dcps_ConditionImpl
 * Method:    jniGetTriggerValue
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL
SAJ_FUNCTION(jniGetTriggerValue)(
    JNIEnv *env,
    jobject jcondition)
{
    gapi_condition condition;
    
    condition = (gapi_condition) saj_read_gapi_address(env, jcondition);
    
    return (jboolean)gapi_condition_get_trigger_value(condition);
}
  
#undef SAJ_FUNCTION