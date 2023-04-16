idf_component_register( SRCS BleManager.cpp BleManager.cpp BleServerCallbacks.cpp
                        INCLUDE_DIRS "include"
                        REQUIRES nimBLE shared-manager)