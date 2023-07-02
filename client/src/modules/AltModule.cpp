#include "Module.h"

// clang-format off
extern js::Class playerClass, localPlayerClass, vehicleClass, pedClass, networkObjectClass,
                audioClass, audioFilterClass, blipClass, textLabelClass, checkpointClass, webViewClass, fontClass,
                localPedClass, localVehicleClass, rmlDocumentClass, rmlElementClass, objectClass, webSocketClientClass,
                mapZoomDataClass, virtualEntityClass, virtualEntityGroupClass;
extern js::Namespace eventsNamespace, discordNamespace, voiceNamespace, localStorageNamespace, statsNamespace;
static js::Module altModule("@altv/client", "@altv/shared",
    { &playerClass, &localPlayerClass, &vehicleClass, &pedClass, &networkObjectClass,
    &audioClass, &audioFilterClass, &blipClass, &textLabelClass, &checkpointClass, &webViewClass, &fontClass,
    &localPedClass, &localVehicleClass, &rmlDocumentClass, &rmlElementClass, &objectClass, &webSocketClientClass,
    &mapZoomDataClass, &virtualEntityClass, &virtualEntityGroupClass },
[](js::ModuleTemplate& module) {
    module.StaticProperty("isClient", true);
    module.StaticProperty("isServer", false);

    module.Namespace(eventsNamespace);
    module.Namespace(discordNamespace);
    module.Namespace(voiceNamespace);
    module.Namespace(localStorageNamespace);
    module.Namespace(statsNamespace);
    module.Namespace("WeaponObject");
});
