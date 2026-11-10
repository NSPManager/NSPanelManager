import createClient, { type Middleware } from "openapi-fetch";
import type { components, paths } from "./schema";

// Typed client for the /rest/v2/* endpoints in docker/web/openapi.yaml. URLs, methods, request bodies
// and responses are checked against the Django serializers at compile time. schema.d.ts is generated:
// run docker/web/generate_api.sh after changing the API (see CONTRIBUTING.md).
//
// openapi-fetch infers `body` generically, so an inline object literal isn't checked for extra keys.
// Pass a typed value instead (e.g. form data from useForm<SettingsUpdateRequest>()). The server rejects
// unknown keys with a 400, so a typo still fails, but at runtime rather than in `tsc`.

export function getCookie(name: string): string {
  for (const cookie of document.cookie ? document.cookie.split(";") : []) {
    const [key, ...value] = cookie.trim().split("=");
    if (key === name) {
      return decodeURIComponent(value.join("="));
    }
  }
  return "";
}

// Django's CSRF check applies to the v2 views as well (web/rest_v2/auth.py).
const csrf: Middleware = {
  onRequest({ request }) {
    if (!["GET", "HEAD", "OPTIONS", "TRACE"].includes(request.method)) {
      request.headers.set("X-CSRFToken", getCookie("csrftoken"));
    }
    return request;
  },
};

// openapi-fetch builds a Request from baseUrl + path, and a Request needs an absolute URL outside a
// browser (e.g. under Vitest). The origin gives the same "/rest/..." URLs the rest of the UI fetches.
// fetch is looked up per request (openapi-fetch would otherwise keep the one present at import), so
// tests can stub it.
export const api = createClient<paths>({
  baseUrl: globalThis.location?.origin ?? "",
  credentials: "same-origin",
  fetch: (request) => globalThis.fetch(request),
});
api.use(csrf);

export type Settings = components["schemas"]["Settings"];
export type SettingsUpdateRequest = components["schemas"]["PatchedSettingsUpdateRequest"];
export type SettingsErrors = components["schemas"]["SettingsErrors"];

export type Room = components["schemas"]["Room"];
export type RoomUpdateRequest = components["schemas"]["PatchedRoomRequest"];
export type RoomErrors = components["schemas"]["RoomErrors"];
