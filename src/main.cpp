#include <iostream>
#include <vector>

#include "openxr/openxr.h"

int main() {
    std::vector<const char *> enabledExtensions = {
            XR_MND_HEADLESS_EXTENSION_NAME
    };

    XrInstance instance;
    {
        XrApplicationInfo applicationInfo = {
                .applicationName = "barebones",
                .applicationVersion = 1,
                .engineName = "",
                .engineVersion = 1,
                .apiVersion = XR_MAKE_VERSION(1, 1, 0),
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

    XrEventDataBuffer event = {XR_TYPE_EVENT_DATA_BUFFER};
    while (XrResult result = xrPollEvent(instance, &event)) {
        switch (event.type) {
            case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                const XrEventDataSessionStateChanged &sessionStateChanged = *reinterpret_cast<XrEventDataSessionStateChanged *>(&event);

                switch (sessionStateChanged.state) {
                    case XR_SESSION_STATE_READY: {

                    }
                }

                break;
            }
        }
    }
}