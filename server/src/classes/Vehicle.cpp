#include "Class.h"
#include "cpp-sdk/ICore.h"

static void Ctor(js::FunctionContext& ctx)
{
    if(!ctx.CheckCtor()) return;
    if(!ctx.CheckArgCount(2, 3)) return;

    uint32_t model;
    if(!ctx.GetArgAsHash(0, model)) return;
    alt::Position pos;
    if(!ctx.GetArg(1, pos)) return;
    alt::Rotation rot{ 0, 0, 0 };
    if(ctx.GetArgCount() == 3 && !ctx.GetArg(2, rot)) return;

    alt::IVehicle* vehicle = alt::ICore::Instance().CreateVehicle(model, pos, rot);
    if(!ctx.Check(vehicle != nullptr, "Failed to create vehicle")) return;

    ctx.SetThisObject(vehicle);
}

static void GetByID(js::FunctionContext& ctx)
{
    if(!ctx.CheckArgCount(1)) return;

    uint16_t id;
    if(!ctx.GetArg(0, id)) return;
    alt::IEntity* entity = alt::ICore::Instance().GetEntityByID(id);
    if(!entity || entity->GetType() != alt::IBaseObject::Type::VEHICLE) ctx.Return(nullptr);
    else
        ctx.Return((alt::IBaseObject*)entity);
}

// clang-format off
extern js::Class sharedVehicleClass;
extern js::Class vehicleClass("Vehicle", &sharedVehicleClass, Ctor, [](js::ClassTemplate& tpl)
{
    tpl.BindToType(alt::IBaseObject::Type::VEHICLE);

    tpl.StaticFunction("getByID", &GetByID);
});
