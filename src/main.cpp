#include <iostream>
#include <vector>

#include "openxr/openxr.h"
#include <thread>

int main() {
    std::vector<const char *> enabledExtensions = {
            XR_MND_HEADLESS_EXTENSION_NAME,
            XR_HTCX_VIVE_TRACKER_INTERACTION_EXTENSION_NAME,
    };

    XrInstance instance;
    {
        XrApplicationInfo applicationInfo = {
                .applicationName = "barebones",
                .applicationVersion = 1,
                .engineName = "",
                .engineVersion = 1,
                .apiVersion = XR_MAKE_VERSION(1, 0, 0),
        };
        XrInstanceCreateInfo instanceCreateInfo = {
                .type = XR_TYPE_INSTANCE_CREATE_INFO,
                .next = nullptr,
                .applicationInfo = applicationInfo,
                .enabledApiLayerCount = 0,
                .enabledExtensionCount = (uint32_t) enabledExtensions.size(),
                .enabledExtensionNames = enabledExtensions.data(),
        };
        if (XrResult result = xrCreateInstance(&instanceCreateInfo, &instance)) {
            std::cout << "Failed to create instance: " << result << std::endl;
            return 1;
        }
    }


    XrInstanceProperties instanceProperties = {XR_TYPE_INSTANCE_PROPERTIES};
    if (xrGetInstanceProperties(instance, &instanceProperties) == XR_SUCCESS) {
        std::cout << "Runtime name: " << instanceProperties.runtimeName << "\n";
        std::cout << "Runtime version: "
                  << XR_VERSION_MAJOR(instanceProperties.runtimeVersion) << "."
                  << XR_VERSION_MAJOR(instanceProperties.runtimeVersion) << "."
                  << XR_VERSION_MAJOR(instanceProperties.runtimeVersion) << "\n";
    }

    XrSystemId systemId;
    XrSystemGetInfo systemGetInfo = {
            .type = XR_TYPE_SYSTEM_GET_INFO,
            .formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY,
    };
    if (XrResult result = xrGetSystem(instance, &systemGetInfo, &systemId)) {
        std::cout << "Failed to get system: " << result << std::endl;
        return 1;
    }

    XrSession session;
    {
        XrSessionCreateInfo sessionCreateInfo = {
                .type = XR_TYPE_SESSION_CREATE_INFO,
                .next = nullptr, //graphicsBinding here if not headless
                .systemId = systemId,
        };
        if (XrResult result = xrCreateSession(instance, &sessionCreateInfo, &session)) {
            std::cout << "Failed to create system: " << result << std::endl;
            return 1;
        }
    }

    XrActionSet actionSet;
    XrActionSetCreateInfo actionSetCreateInfo = {
            .type = XR_TYPE_ACTION_SET_CREATE_INFO,
            .actionSetName = "actionsettest",
            .localizedActionSetName= "Action Set Test",
            .priority = 0,
    };
    if(XrResult result = xrCreateActionSet(instance, &actionSetCreateInfo, &actionSet)) {
        std::cout << "Failed to create action set: " << result << std::endl;
        return 1;
    }

    XrPath trackerChest;
    if(XrResult result = xrStringToPath(instance, "/user/vive_tracker_htcx/role/chest", &trackerChest)) {
        std::cout << "String to path error: " << result << std::endl;
        return 1;
    }

    XrAction viveAction1;
    {
        XrActionCreateInfo actionCreateInfo = {
                .type = XR_TYPE_ACTION_CREATE_INFO,
                .actionName = "viveaction1",
                .actionType = XR_ACTION_TYPE_POSE_INPUT,
                .countSubactionPaths = 1,
                .subactionPaths = &trackerChest,
                .localizedActionName = "Vive Action Test 1",
        };
        if(XrResult result = xrCreateAction(actionSet, &actionCreateInfo, &viveAction1)) {
            std::cout << "Failed to create action " << result << std::endl;
            return 1;
        }
    }

    XrAction viveAction2;
    {
        XrActionCreateInfo actionCreateInfo = {
                .type = XR_TYPE_ACTION_CREATE_INFO,
                .actionName = "viveaction2",
                .actionType = XR_ACTION_TYPE_BOOLEAN_INPUT,
                .countSubactionPaths = 1,
                .subactionPaths = &trackerChest,
                .localizedActionName = "Vive Action Test 2",
        };
        if(XrResult result = xrCreateAction(actionSet, &actionCreateInfo, &viveAction2)) {
            std::cout << "Failed to create action " << result << std::endl;
            return 1;
        }
    }

    XrPath viveTrackerInteractionProfilePath;
    if(XrResult result = xrStringToPath(instance, "/interaction_profiles/htc/vive_tracker_htcx", &viveTrackerInteractionProfilePath)) {
        std::cout << "String to path error: " << result << std::endl;
        return 1;
    }

    XrPath viveTrackerGrip;
    if(XrResult result = xrStringToPath(instance, "/user/vive_tracker_htcx/role/chest/input/grip/pose", &viveTrackerGrip)) {
        std::cout << "String to path error: " << result << std::endl;
        return 1;
    }

    XrPath viveTrackerTriggerClick;
    if(XrResult result = xrStringToPath(instance, "/user/vive_tracker_htcx/role/chest/input/trigger/click", &viveTrackerTriggerClick)) {
        std::cout << "String to path error: " << result << std::endl;
        return 1;
    }

    std::vector<XrActionSuggestedBinding> suggestedBindings = {
            {
                    .action = viveAction2,
                    .binding = viveTrackerTriggerClick
            },
            {
                    .action = viveAction1,
                    .binding = viveTrackerGrip,
            }
    };

    XrInteractionProfileSuggestedBinding viveTrackerBindings = {
            .type = XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING,
            .interactionProfile = viveTrackerInteractionProfilePath,
            .countSuggestedBindings = (uint32_t ) suggestedBindings.size(),
            .suggestedBindings = suggestedBindings.data(),
    };
    if(XrResult result = xrSuggestInteractionProfileBindings(instance, &viveTrackerBindings)) {
        std::cout << "Failed to suggest bindings: " << result << std::endl;
        return 1;
    }

    XrEventDataBuffer event = {XR_TYPE_EVENT_DATA_BUFFER};
    while(true) {
        while (XrResult result = xrPollEvent(instance, &event)) {
            switch (event.type) {
                case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                    const XrEventDataSessionStateChanged &sessionStateChanged = *reinterpret_cast<XrEventDataSessionStateChanged *>(&event);

                    switch (sessionStateChanged.state) {
                        case XR_SESSION_STATE_READY: {
                            XrSessionActionSetsAttachInfo attachInfo = {
                                    .type = XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO,
                                    .countActionSets = 1,
                                    .actionSets = &actionSet,
                            };
                            if(XrResult result2 = xrAttachSessionActionSets(session, &attachInfo)) {
                                std::cout << "Failed to attach action sets: " << result2 << std::endl;
                                return 1;
                            }

                            break;
                        }
                    }

                    break;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

}