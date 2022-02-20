#pragma once

enum class MessageID { PushState, PopToState, PopAndPushState, CameraShake };

#define EVENT_MESSAGE_ID_TYPE MessageID
#define EVENT_LISTENER_ID_TYPE void*