import createClient from "openapi-fetch";
import type { components, paths } from "./schema";

// Typed client for the /rest/* endpoints in docker/web/openapi.yaml. URLs, methods, request
// bodies and responses are checked against the Django serializers at compile time.
// schema.d.ts is generated: run docker/web/generate_api.sh after changing the API.
// openapi-fetch infers `body` generically, so an inline object literal isn't checked for extra
// keys. Pass a typed value instead (e.g. form data from useForm<RoomUpdateRequest>); the server
// only logs a warning for unknown fields, so a typo there fails quietly.
export const api = createClient<paths>({ credentials: "same-origin" });

export type Room = components["schemas"]["Room"];
export type RoomUpdateRequest = components["schemas"]["RoomUpdateRequest"];
