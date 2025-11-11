# Homey Integration for NSPanelManager

## Overview
This document outlines the comprehensive integration of Homey Web API into NSPanelManager to enable control of Homey devices (lights, switches, buttons) and scenes (Flows and Moods). This integration follows the same architecture pattern as existing Home Assistant and OpenHAB integrations.

---

## 1. Requirements

### 1.1 Supported Entities
- **Lights**: Devices with capabilities `onoff`, `dim`, `light_hue`, `light_saturation`, `light_temperature`, `light_mode`
- **Switches**: Devices with capability `onoff`
- **Buttons**: Devices with capability `button`
- **Scenes**: Homey Flows (prefixed with `[F]`) and Moods (prefixed with `[M]`)

### 1.2 Configuration
- Homey IP address or mDNS name (e.g., `homey.local`)
- Homey API key (generated in Homey app)
- Settings stored in NSPanelManager database

### 1.3 Real-time Updates
- WebSocket connection for real-time device state synchronization
- Event-driven architecture similar to Home Assistant integration

### 1.4 UI/UX Requirements
- Homey as third integration option alongside HA and OpenHAB
- Settings configuration page for Homey credentials
- Entity selection and filtering by capability
- Scene naming with [F] and [M] prefixes

---

## 2. Architecture Overview

```
┌─────────────────────────────────┐
│   Web Interface (Python)         │
├─────────────────────────────────┤
│ • homey_api.py (HTTP requests)   │
│ • api.py (extend entity fetching)│
│ • htmx.py (UI integration)       │
└──────────────┬──────────────────┘
               │
        ┌──────┴──────────┐
        │                 │
   ┌────▼─────┐    ┌──────▼──────────┐
   │ SQLite   │    │ Homey API       │
   │ Database │    │ (HTTP/REST)     │
   └──────────┘    └──────┬──────────┘
                          │
┌─────────────────────────┴───────────────────────┐
│   MQTTManager (C++)                             │
├─────────────────────────────────────────────────┤
│ • HomeyManager (WebSocket)                      │
│ • HomeyLight / HomeySwitch / HomeyThermostat    │
│ • HomeyButton / HomeyScene                      │
│ • EntityManager (instantiation logic)           │
└─────────────────────────────────────────────────┘
```

---

## 3. Implementation Phases

### Phase 1: Foundation & Settings
**Status**: 🟨 In Progress (Python API client completed)

#### 3.1.1 Settings Configuration
- [ ] Add `homey_address` setting to database
- [ ] Add `homey_token` setting to database
- [ ] Create settings UI in web interface
- [ ] Add Homey section to initial setup wizard
- [ ] Implement settings persistence

#### 3.1.2 Documentation
- [ ] Create API key generation guide
- [ ] Document Homey IP address configuration
- [ ] Document settings validation

**Deliverables**:
- ✅ Python homey_api.py module created with device/flow/mood discovery
- Settings stored in web_settings table
- UI form for Homey configuration
- Initial setup step for Homey

---

### Phase 2: Python Web Interface - Device Discovery
**Status**: 🟨 In Progress (API client and core integration done)

#### 3.2.1 Homey API Client
- [x] Create `docker/web/nspanelmanager/web/homey_api.py`
- [x] Implement `get_all_homey_devices()`
- [x] Implement capability filtering logic
- [x] Implement `get_all_homey_flows()`
- [x] Implement `get_all_homey_moods()`
- [x] Implement error handling and logging

#### 3.2.2 API Integration
- [x] Add Homey to `get_all_available_entities()` in `api.py`
- [x] Filter devices by supported capabilities
- [x] Add [F] prefix to Flow names
- [x] Add [M] prefix to Mood names
- [x] Return standardized entity format

#### 3.2.3 Entity Addition UI
- [ ] Extend entity source selection in `htmx.py`
- [ ] Add Homey option to `partial_add_entity_to_entities_page_select_entity_source()`
- [ ] Add Homey to supported entity types lists
- [ ] Implement entity-specific configuration screens

**Deliverables**:
- ✅ `homey_api.py` module with device discovery
- ✅ Integration with `get_all_available_entities()` in api.py
- ⏳ Ability to fetch and display Homey devices/scenes (pending htmx.py UI)

---

### Phase 3: Entity Creation & Storage
**Status**: ⬜ Not Started

#### 3.3.1 Light Entity Creation
- [ ] Extend `partial_entity_add_light_entity()` for Homey
- [ ] Extend `create_or_update_light_entity()` for Homey
- [ ] Store Homey device ID in entity_data JSON
- [ ] Store capability list in entity_data JSON
- [ ] Map light properties (can_dim, can_color_temperature, can_rgb)

#### 3.3.2 Switch Entity Creation
- [ ] Extend `partial_entity_add_switch_entity()` for Homey
- [ ] Extend `create_or_update_switch_entity()` for Homey
- [ ] Store Homey device ID in entity_data JSON

#### 3.3.3 Button Entity Creation
- [ ] Extend `partial_entity_add_button_entity()` for Homey
- [ ] Extend `create_or_update_button_entity()` for Homey
- [ ] Store Homey device ID in entity_data JSON

#### 3.3.4 Scene Entity Creation
- [ ] Extend `partial_entity_add_scene_entity()` for Homey
- [ ] Extend `create_or_update_scene_entity()` for Homey
- [ ] Store Homey flow/mood ID in backend_name
- [ ] Store type indicator (flow/mood) for display

**Entity Data JSON Structures**:

Light:
```json
{
  "controller": "homey",
  "homey_device_id": "device-uuid",
  "can_dim": true,
  "can_color_temperature": true,
  "can_rgb": true,
  "capabilities": ["onoff", "dim", "light_temperature", "light_hue", "light_saturation"]
}
```

Switch:
```json
{
  "controller": "homey",
  "homey_device_id": "device-uuid",
  "capabilities": ["onoff"]
}
```

Button:
```json
{
  "controller": "homey",
  "homey_device_id": "device-uuid",
  "capabilities": ["button"]
}
```

Scene:
```json
{
  "controller": "homey",
  "homey_id": "flow-or-mood-uuid",
  "homey_type": "flow"
}
```

**Deliverables**:
- Entity creation UI for Homey entities
- Proper storage of Homey-specific data
- Database records with correct controller type

---

### Phase 4: C++ Backend - Homey Manager
**Status**: ⬜ Not Started

#### 3.4.1 HomeyManager Header
- [ ] Create `docker/MQTTManager/include/homey_manager/homey_manager.hpp`
- [ ] Define class structure (static methods, WebSocket, events)
- [ ] Define configuration struct
- [ ] Define observer/signal pattern

#### 3.4.2 HomeyManager Implementation
- [ ] Implement `init()` - Start thread and WebSocket
- [ ] Implement `connect()` - Connect to Homey WebSocket
- [ ] Implement `reload_config()` - Reload from database
- [ ] Implement authentication with API key
- [ ] Implement WebSocket message handling
- [ ] Implement device event processing
- [ ] Implement observer pattern for entity updates
- [ ] Implement disconnect/reconnect logic
- [ ] Add comprehensive logging

#### 3.4.3 Configuration Management
- [ ] Load homey_address from settings
- [ ] Load homey_token from settings
- [ ] Handle configuration changes
- [ ] Validate credentials

**Deliverables**:
- Functional HomeyManager with WebSocket connection
- Real-time device event handling
- Observer pattern for entity state changes

---

### Phase 5: C++ Backend - Entity Types
**Status**: ⬜ Not Started

#### 3.5.1 Homey Light
- [ ] Create `docker/MQTTManager/include/light/homey_light.hpp`
- [ ] Create `docker/MQTTManager/include/light/homey_light.cpp`
- [ ] Extend Light base class
- [ ] Implement state synchronization
- [ ] Implement brightness control
- [ ] Implement color temperature control
- [ ] Implement RGB color control
- [ ] Implement hue/saturation control
- [ ] Handle device events from HomeyManager
- [ ] Implement `send_state_update_to_controller()`

#### 3.5.2 Homey Switch
- [ ] Create `docker/MQTTManager/include/switch/homey_switch.hpp`
- [ ] Create `docker/MQTTManager/include/switch/homey_switch.cpp`
- [ ] Extend SwitchEntity base class
- [ ] Implement on/off control
- [ ] Handle device events
- [ ] Implement `send_state_update_to_controller()`

#### 3.5.3 Homey Button
- [ ] Create `docker/MQTTManager/include/button/homey_button.hpp`
- [ ] Create `docker/MQTTManager/include/button/homey_button.cpp`
- [ ] Extend ButtonEntity base class
- [ ] Implement button press triggering
- [ ] Handle device events
- [ ] Implement `send_state_update_to_controller()`

#### 3.5.4 Homey Scene
- [ ] Create `docker/MQTTManager/include/scenes/homey_scene.hpp`
- [ ] Create `docker/MQTTManager/include/scenes/homey_scene.cpp`
- [ ] Extend Scene base class
- [ ] Support both Flows and Moods
- [ ] Implement scene activation
- [ ] Store and detect scene type (flow/mood)

**Deliverables**:
- All four entity types implemented
- Full control capabilities
- Proper state synchronization

---

### Phase 6: Integration with EntityManager
**Status**: ⬜ Not Started

#### 3.6.1 EntityManager Modifications
- [ ] Modify `load_lights()` - Add Homey light instantiation
- [ ] Modify `load_switches()` - Add Homey switch instantiation
- [ ] Modify `load_buttons()` - Add Homey button instantiation
- [ ] Modify `load_scenes()` - Add Homey scene instantiation
- [ ] Add includes for new Homey entity headers

#### 3.6.2 CMake Updates
- [ ] Add Homey source files to CMakeLists.txt
- [ ] Ensure compilation of new modules
- [ ] Verify dependency linking

#### 3.6.3 Initialization
- [ ] Initialize HomeyManager in application startup
- [ ] Ensure proper thread management
- [ ] Handle shutdown gracefully

**Deliverables**:
- EntityManager properly instantiates Homey entities
- Project compiles with all new files
- HomeyManager starts on application launch

---

### Phase 7: Testing & Debugging
**Status**: ⬜ Not Started

#### 3.7.1 Unit Testing
- [ ] Test Homey API connectivity
- [ ] Test capability filtering
- [ ] Test entity data storage/retrieval
- [ ] Test WebSocket connection handling
- [ ] Test device state synchronization

#### 3.7.2 Integration Testing
- [ ] Test light control (on/off, brightness, color)
- [ ] Test switch control
- [ ] Test button triggering
- [ ] Test scene activation
- [ ] Test real-time updates
- [ ] Test reconnection logic
- [ ] Test configuration changes

#### 3.7.3 Debugging
- [ ] Verify database records
- [ ] Check WebSocket logs
- [ ] Monitor entity state changes
- [ ] Test error scenarios
- [ ] Validate API compatibility

**Deliverables**:
- Comprehensive test coverage
- Bug fixes and refinements
- Performance validation

---

### Phase 8: Documentation & UX Polish
**Status**: ⬜ Not Started

#### 3.8.1 User Documentation
- [ ] Create Homey setup guide
- [ ] Document API key generation
- [ ] Document supported device types
- [ ] Create troubleshooting guide

#### 3.8.2 Code Documentation
- [ ] Document HomeyManager API
- [ ] Document entity classes
- [ ] Add code comments for complex logic
- [ ] Create architecture diagrams

#### 3.8.3 UX Improvements
- [ ] Improve error messages
- [ ] Add connection status indicators
- [ ] Enhance entity filtering UI
- [ ] Add validation feedback

**Deliverables**:
- Complete user documentation
- Code documentation
- Polished UI/UX

---

## 4. File Structure Changes

### New Files
```
docker/web/nspanelmanager/web/
├── homey_api.py                              (NEW)

docker/MQTTManager/include/
├── homey_manager/
│   ├── homey_manager.hpp                     (NEW)
│   └── homey_manager.cpp                     (NEW)
├── light/
│   ├── homey_light.hpp                       (NEW)
│   └── homey_light.cpp                       (NEW)
├── switch/
│   ├── homey_switch.hpp                      (NEW)
│   └── homey_switch.cpp                      (NEW)
├── button/
│   ├── homey_button.hpp                      (NEW)
│   └── homey_button.cpp                      (NEW)
└── scenes/
    ├── homey_scene.hpp                       (NEW)
    └── homey_scene.cpp                       (NEW)
```

### Modified Files
```
docker/web/nspanelmanager/web/
├── api.py                                    (MODIFIED)
├── htmx.py                                   (MODIFIED)

docker/MQTTManager/
├── CMakeLists.txt                            (MODIFIED)
├── include/entity_manager/
│   ├── entity_manager.hpp                    (MODIFIED)
│   └── entity_manager.cpp                    (MODIFIED)
```

---

## 5. Homey API Integration Details

### 5.1 REST API Endpoints (Python)

#### Get All Devices
```
GET http://{homey_address}/api/manager/devices/device
Headers: Authorization: Bearer {api_key}
```

Response includes device data with capabilities.

#### Get All Flows
```
GET http://{homey_address}/api/manager/flow/flow
Headers: Authorization: Bearer {api_key}
```

#### Get All Moods
Access via devices or dedicated mood endpoints.

#### Trigger Device Capability (for control)
```
PUT http://{homey_address}/api/manager/devices/device/{device_id}/capability/{capability_id}
Headers: Authorization: Bearer {api_key}
Body: { "value": <capability_value> }
```

#### Trigger Flow
```
POST http://{homey_address}/api/manager/flow/flow/{flow_id}/trigger
Headers: Authorization: Bearer {api_key}
```

### 5.2 WebSocket API (C++)

#### Connection
```
ws://{homey_address}/api/manager/devices/device?token={api_key}
```

#### Message Format
```json
{
  "type": "device.update",
  "device": {
    "id": "device-uuid",
    "capabilities": {
      "onoff": { "value": true },
      "dim": { "value": 0.85 },
      "light_temperature": { "value": 4000 }
    }
  }
}
```

---

## 6. Capability Mapping

| Homey Capability    | NSPanel Feature | Data Type       | Range               |
| ------------------- | --------------- | --------------- | ------------------- |
| `onoff`             | On/Off toggle   | Boolean         | true/false          |
| `dim`               | Brightness      | Float (decimal) | 0.0 - 1.0 → 0-100%  |
| `light_hue`         | Hue             | Float           | 0.0 - 1.0 → 0-360°  |
| `light_saturation`  | Saturation      | Float           | 0.0 - 1.0 → 0-100%  |
| `light_temperature` | Color Temp      | Integer         | Kelvin (1000-10000) |
| `light_mode`        | Light Mode      | String          | (mode-specific)     |
| `button`            | Button Press    | (trigger)       | (event-based)       |

---

## 7. Entity Data Schema

### Database Schema (No changes required)
Uses existing Entity and Scene tables with controller field in entity_data JSON.

### entity_data JSON Schema

**Light Entity**:
```json
{
  "controller": "homey",
  "homey_device_id": "<uuid>",
  "homey_device_name": "<name>",
  "can_dim": true,
  "can_color_temperature": true,
  "can_rgb": true,
  "capabilities": ["onoff", "dim", "light_temperature", "light_hue", "light_saturation"],
  "controlled_by_nspanel_main_page": true,
  "is_ceiling_light": false
}
```

**Switch Entity**:
```json
{
  "controller": "homey",
  "homey_device_id": "<uuid>",
  "homey_device_name": "<name>",
  "capabilities": ["onoff"]
}
```

**Button Entity**:
```json
{
  "controller": "homey",
  "homey_device_id": "<uuid>",
  "homey_device_name": "<name>",
  "capabilities": ["button"]
}
```

**Scene (Backend Name)**:
```
homey_flow_<uuid> (for Flows)
homey_mood_<uuid> (for Moods)
```

---

## 8. Progress Checklist

### Summary Status: 🟨 In Progress (2/8 phases started, 10/32 tasks completed)

- 🟨 **Phase 1**: Foundation & Settings (1/2 sections, API client done)
- 🟨 **Phase 2**: Python Web Interface (2/3 sections, API + core integration done)
- [ ] **Phase 3**: Entity Creation & Storage (0/4 sections)
- [ ] **Phase 4**: C++ Backend - Homey Manager (0/3 sections)
- [ ] **Phase 5**: C++ Backend - Entity Types (0/4 sections)
- [ ] **Phase 6**: Integration with EntityManager (0/3 sections)
- [ ] **Phase 7**: Testing & Debugging (0/3 sections)
- [ ] **Phase 8**: Documentation & UX Polish (0/3 sections)

### Recently Completed
- ✅ Created comprehensive HOMEY_INTEGRATION.md documentation
- ✅ Created homey_api.py with device/flow/mood discovery
- ✅ Extended api.py to integrate Homey entity fetching

---

## 9. Additional Notes

### Future Enhancements
- Abstract interface for easier integration of new controllers
- Homey device discovery via mDNS
- API rate limiting and caching
- Homey webhook support for even faster updates
- Multi-Homey support

### Known Limitations
- Initially single Homey instance (can be extended later)
- Button entity only supports basic press (not long press/double press initially)
- Moods treated as scenes (same behavior as Flows)

### Dependencies
- None additional (all libraries already in project)
  - Python: requests (existing)
  - C++: ixwebsocket (existing), nlohmann/json (existing)

---

## 10. References

### Homey API Documentation
- [Homey API V3 Local](https://athombv.github.io/node-homey-api/HomeyAPIV3Local.html)
- [Getting Started with API Keys](https://support.homey.app/hc/en-us/articles/8178797067292-Getting-started-with-API-Keys)
- [ManagerDevices.Device](https://athombv.github.io/node-homey-api/HomeyAPIV3Local.ManagerDevices.Device.html)
- [ManagerDevices.Capability](https://athombv.github.io/node-homey-api/HomeyAPIV3Local.ManagerDevices.Capability.html)

### Project References
- Home Assistant integration: `docker/MQTTManager/include/home_assistant_manager/`
- OpenHAB integration: `docker/MQTTManager/include/openhab_manager/`
