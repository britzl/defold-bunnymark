#define EXTENSION_NAME NativeSetPosition
#define LIB_NAME "NativeSetPosition"
#define MODULE_NAME "native_set_position"

#include <dmsdk/sdk.h>

#define M_PI 3.14159265358979323846

#define USERDATA_METATABLE "POSITION_SETTER_USERDATA"

struct InstancePositionData {
    dmGameObject::HInstance rootInstance;
    dmVMath::Vector3 *position;
    float velocity;
};

class PositionSetterUserdata {
  public:
    dmArray<InstancePositionData> instances;
    explicit PositionSetterUserdata();
    ~PositionSetterUserdata();
    void addInstance(dmGameObject::HInstance rootInstance, dmVMath::Vector3 *position,float velocity);
    void update();
    void updateVelocity(float dt);
    void removeInstance(dmGameObject::HInstance rootInstance);
};

PositionSetterUserdata::PositionSetterUserdata() {
}

PositionSetterUserdata::~PositionSetterUserdata() {
}

void PositionSetterUserdata::addInstance(dmGameObject::HInstance rootInstance, dmVMath::Vector3 *position, float velocity) {
    InstancePositionData instanceVector;
    instanceVector.rootInstance = rootInstance;
    instanceVector.position = position;
    instanceVector.velocity = velocity;
    if (instances.Full()) {
        instances.OffsetCapacity(32);
    }
    instances.Push(instanceVector);
}

void PositionSetterUserdata::update() {
    for (int i = 0; i < instances.Size(); ++i) {
        InstancePositionData instancePositionData = instances[i];
        dmGameObject::SetPosition(instancePositionData.rootInstance, dmVMath::Point3(*instancePositionData.position));
    }
}

void PositionSetterUserdata::updateVelocity(float dt) {
    for (int i = 0; i < instances.Size(); ++i) {
        InstancePositionData instancePositionData = instances[i];
        float velocity = instancePositionData.velocity;
        dmVMath::Vector3 *position = instancePositionData.position;
        velocity -= 1200 * dt;

        // Update position
        position->setY(position->getY() + velocity * dt);
        if (position->getY() < 50) {
            position->setY(50);
            velocity = -velocity;
        }

        // Update velocity in struct
        instances[i].velocity = velocity;

        dmGameObject::SetPosition(instancePositionData.rootInstance, dmVMath::Point3(*instancePositionData.position));
    }
}

void PositionSetterUserdata::removeInstance(dmGameObject::HInstance rootInstance) {
    for (int i = 0; i < instances.Size(); ++i) {
        InstancePositionData instancePositionData = instances[i];
        if (instancePositionData.rootInstance == rootInstance) {
            instances.EraseSwap(i);
            return;
        }
    }
}

static PositionSetterUserdata *PositionSetterUserdataCheck(lua_State *L, int index) {
    return *(PositionSetterUserdata **)luaL_checkudata(L, index, USERDATA_METATABLE);
}

static int LuaPositionSetterUserdataAddInstance(lua_State *L) {
    PositionSetterUserdata *userdata = PositionSetterUserdataCheck(L, 1);
    dmGameObject::HInstance rootInstance = dmScript::CheckGOInstance(L, 2);
    dmVMath::Vector3 *position = dmScript::CheckVector3(L, 3);
    float velocity = luaL_checknumber(L, 4);
    userdata->addInstance(rootInstance, position, velocity);
    return 0;
}

static int LuaPositionSetterUserdataUpdate(lua_State *L) {
    PositionSetterUserdata *userdata = PositionSetterUserdataCheck(L, 1);
    userdata->update();
    return 0;
}

static int LuaPositionSetterUserdataUpdateVelocity(lua_State *L) {
    PositionSetterUserdata *userdata = PositionSetterUserdataCheck(L, 1);
    userdata->updateVelocity(luaL_checknumber(L, 2));
    return 0;
}

static int LuaPositionSetterUserdataRemoveInstance(lua_State *L) {
    PositionSetterUserdata *userdata = PositionSetterUserdataCheck(L, 1);
    dmGameObject::HInstance rootInstance = dmScript::CheckGOInstance(L, 2);
    userdata->removeInstance(rootInstance);
    return 0;
}

static int LuaPositionSetterUserdataGC(lua_State *L) {
    PositionSetterUserdata *userdata = PositionSetterUserdataCheck(L, 1);
    delete userdata;
    return 0;
}

static const luaL_Reg LuaPositionSetterUserdataMethods[] = {
    {"__gc", LuaPositionSetterUserdataGC},
    {"add_instance", LuaPositionSetterUserdataAddInstance},
    {"remove_instance", LuaPositionSetterUserdataRemoveInstance},
    {"update", LuaPositionSetterUserdataUpdate},
    {"update_velocity", LuaPositionSetterUserdataUpdateVelocity},
    {0, 0}};

int NewPositionSetterUserdataLua(lua_State *L) {
    PositionSetterUserdata *userdata = new PositionSetterUserdata();
    PositionSetterUserdata **ud = (PositionSetterUserdata **)lua_newuserdata(L, sizeof(PositionSetterUserdata *));
    *ud = userdata;
    if (luaL_newmetatable(L, USERDATA_METATABLE)) {
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
        luaL_register(L, 0, LuaPositionSetterUserdataMethods);
    }
    lua_setmetatable(L, -2);
    return 1;
}

// Functions exposed to Lua
static const luaL_reg Module_methods[] = {
    {"new_position_setter", NewPositionSetterUserdataLua},
    {0, 0}};

static void LuaInit(lua_State *L) {
    int top = lua_gettop(L);
    luaL_register(L, MODULE_NAME, Module_methods);
    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

static dmExtension::Result AppInitializeMyExtension(dmExtension::AppParams *params) { return dmExtension::RESULT_OK; }
static dmExtension::Result InitializeMyExtension(dmExtension::Params *params) {
    // Init Lua
    LuaInit(params->m_L);

    printf("Registered %s Extension\n", MODULE_NAME);
    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeMyExtension(dmExtension::AppParams *params) { return dmExtension::RESULT_OK; }

static dmExtension::Result FinalizeMyExtension(dmExtension::Params *params) { return dmExtension::RESULT_OK; }

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, AppInitializeMyExtension, AppFinalizeMyExtension, InitializeMyExtension, 0, 0, FinalizeMyExtension)