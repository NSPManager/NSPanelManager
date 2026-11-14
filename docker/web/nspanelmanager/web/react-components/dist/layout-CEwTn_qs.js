import { r as require_react, t as require_jsx_runtime } from "./main-DsrUEgTV.js";
//#region node_modules/openapi-fetch/dist/index.mjs
var PATH_PARAM_RE = /\{[^{}]+\}/g;
var supportsRequestInitExt = () => {
	return typeof process === "object" && Number.parseInt(process?.versions?.node?.substring(0, 2)) >= 18 && process.versions.undici;
};
function randomID() {
	return Math.random().toString(36).slice(2, 11);
}
function createClient(clientOptions) {
	let { baseUrl = "", Request: CustomRequest = globalThis.Request, fetch: baseFetch = globalThis.fetch, querySerializer: globalQuerySerializer, bodySerializer: globalBodySerializer, pathSerializer: globalPathSerializer, headers: baseHeaders, requestInitExt = void 0, ...baseOptions } = { ...clientOptions };
	requestInitExt = supportsRequestInitExt() ? requestInitExt : void 0;
	baseUrl = removeTrailingSlash(baseUrl);
	const globalMiddlewares = [];
	async function coreFetch(schemaPath, fetchOptions) {
		const { baseUrl: localBaseUrl, fetch = baseFetch, Request = CustomRequest, headers, params = {}, parseAs = "json", querySerializer: requestQuerySerializer, bodySerializer = globalBodySerializer ?? defaultBodySerializer, pathSerializer: requestPathSerializer, body, middleware: requestMiddlewares = [], ...init } = fetchOptions || {};
		let finalBaseUrl = baseUrl;
		if (localBaseUrl) finalBaseUrl = removeTrailingSlash(localBaseUrl) ?? baseUrl;
		let querySerializer = typeof globalQuerySerializer === "function" ? globalQuerySerializer : createQuerySerializer(globalQuerySerializer);
		if (requestQuerySerializer) querySerializer = typeof requestQuerySerializer === "function" ? requestQuerySerializer : createQuerySerializer({
			...typeof globalQuerySerializer === "object" ? globalQuerySerializer : {},
			...requestQuerySerializer
		});
		const pathSerializer = requestPathSerializer || globalPathSerializer || defaultPathSerializer;
		const serializedBody = body === void 0 ? void 0 : bodySerializer(body, mergeHeaders(baseHeaders, headers, params.header));
		const finalHeaders = mergeHeaders(serializedBody === void 0 || serializedBody instanceof FormData ? {} : { "Content-Type": "application/json" }, baseHeaders, headers, params.header);
		const finalMiddlewares = [...globalMiddlewares, ...requestMiddlewares];
		const requestInit = {
			redirect: "follow",
			...baseOptions,
			...init,
			body: serializedBody,
			headers: finalHeaders
		};
		let id;
		let options;
		let request = new Request(createFinalURL(schemaPath, {
			baseUrl: finalBaseUrl,
			params,
			querySerializer,
			pathSerializer
		}), requestInit);
		let response;
		for (const key in init) if (!(key in request)) request[key] = init[key];
		if (finalMiddlewares.length) {
			id = randomID();
			options = Object.freeze({
				baseUrl: finalBaseUrl,
				fetch,
				parseAs,
				querySerializer,
				bodySerializer,
				pathSerializer
			});
			for (const m of finalMiddlewares) if (m && typeof m === "object" && typeof m.onRequest === "function") {
				const result = await m.onRequest({
					request,
					schemaPath,
					params,
					options,
					id
				});
				if (result) {
					if (result instanceof Request) request = result;
					else if (result instanceof Response) {
						response = result;
						break;
					} else throw new Error("onRequest: must return new Request() or Response() when modifying the request");
				}
			}
		}
		if (!response) {
			try {
				response = await fetch(request, requestInitExt);
			} catch (error2) {
				let errorAfterMiddleware = error2;
				if (finalMiddlewares.length) for (let i = finalMiddlewares.length - 1; i >= 0; i--) {
					const m = finalMiddlewares[i];
					if (m && typeof m === "object" && typeof m.onError === "function") {
						const result = await m.onError({
							request,
							error: errorAfterMiddleware,
							schemaPath,
							params,
							options,
							id
						});
						if (result) {
							if (result instanceof Response) {
								errorAfterMiddleware = void 0;
								response = result;
								break;
							}
							if (result instanceof Error) {
								errorAfterMiddleware = result;
								continue;
							}
							throw new Error("onError: must return new Response() or instance of Error");
						}
					}
				}
				if (errorAfterMiddleware) throw errorAfterMiddleware;
			}
			if (finalMiddlewares.length) for (let i = finalMiddlewares.length - 1; i >= 0; i--) {
				const m = finalMiddlewares[i];
				if (m && typeof m === "object" && typeof m.onResponse === "function") {
					const result = await m.onResponse({
						request,
						response,
						schemaPath,
						params,
						options,
						id
					});
					if (result) {
						if (!(result instanceof Response)) throw new Error("onResponse: must return new Response() when modifying the response");
						response = result;
					}
				}
			}
		}
		const contentLength = response.headers.get("Content-Length");
		if (response.status === 204 || request.method === "HEAD" || contentLength === "0" && !response.headers.get("Transfer-Encoding")?.includes("chunked")) return response.ok ? {
			data: void 0,
			response
		} : {
			error: void 0,
			response
		};
		if (response.ok) {
			const getResponseData = async () => {
				if (parseAs === "stream") return response.body;
				if (parseAs === "json" && !contentLength) {
					const raw = await response.text();
					return raw ? JSON.parse(raw) : void 0;
				}
				return await response[parseAs]();
			};
			return {
				data: await getResponseData(),
				response
			};
		}
		let error = await response.text();
		try {
			error = JSON.parse(error);
		} catch {}
		return {
			error,
			response
		};
	}
	return {
		request(method, url, init) {
			return coreFetch(url, {
				...init,
				method: method.toUpperCase()
			});
		},
		/** Call a GET endpoint */
		GET(url, init) {
			return coreFetch(url, {
				...init,
				method: "GET"
			});
		},
		/** Call a PUT endpoint */
		PUT(url, init) {
			return coreFetch(url, {
				...init,
				method: "PUT"
			});
		},
		/** Call a POST endpoint */
		POST(url, init) {
			return coreFetch(url, {
				...init,
				method: "POST"
			});
		},
		/** Call a DELETE endpoint */
		DELETE(url, init) {
			return coreFetch(url, {
				...init,
				method: "DELETE"
			});
		},
		/** Call a OPTIONS endpoint */
		OPTIONS(url, init) {
			return coreFetch(url, {
				...init,
				method: "OPTIONS"
			});
		},
		/** Call a HEAD endpoint */
		HEAD(url, init) {
			return coreFetch(url, {
				...init,
				method: "HEAD"
			});
		},
		/** Call a PATCH endpoint */
		PATCH(url, init) {
			return coreFetch(url, {
				...init,
				method: "PATCH"
			});
		},
		/** Call a TRACE endpoint */
		TRACE(url, init) {
			return coreFetch(url, {
				...init,
				method: "TRACE"
			});
		},
		/** Register middleware */
		use(...middleware) {
			for (const m of middleware) {
				if (!m) continue;
				if (typeof m !== "object" || !("onRequest" in m || "onResponse" in m || "onError" in m)) throw new Error("Middleware must be an object with one of `onRequest()`, `onResponse() or `onError()`");
				globalMiddlewares.push(m);
			}
		},
		/** Unregister middleware */
		eject(...middleware) {
			for (const m of middleware) {
				const i = globalMiddlewares.indexOf(m);
				if (i !== -1) globalMiddlewares.splice(i, 1);
			}
		}
	};
}
function serializePrimitiveParam(name, value, options) {
	if (value === void 0 || value === null) return "";
	if (typeof value === "object") throw new Error("Deeply-nested arrays/objects aren’t supported. Provide your own `querySerializer()` to handle these.");
	return `${name}=${options?.allowReserved === true ? value : encodeURIComponent(value)}`;
}
function serializeObjectParam(name, value, options) {
	if (!value || typeof value !== "object") return "";
	const values = [];
	const joiner = {
		simple: ",",
		label: ".",
		matrix: ";"
	}[options.style] || "&";
	if (options.style !== "deepObject" && options.explode === false) {
		for (const k in value) values.push(k, options.allowReserved === true ? value[k] : encodeURIComponent(value[k]));
		const final2 = values.join(",");
		switch (options.style) {
			case "form": return `${name}=${final2}`;
			case "label": return `.${final2}`;
			case "matrix": return `;${name}=${final2}`;
			default: return final2;
		}
	}
	for (const k in value) {
		const finalName = options.style === "deepObject" ? `${name}[${k}]` : k;
		values.push(serializePrimitiveParam(finalName, value[k], options));
	}
	const final = values.join(joiner);
	return options.style === "label" || options.style === "matrix" ? `${joiner}${final}` : final;
}
function serializeArrayParam(name, value, options) {
	if (!Array.isArray(value)) return "";
	if (options.explode === false) {
		const joiner2 = {
			form: ",",
			spaceDelimited: "%20",
			pipeDelimited: "|"
		}[options.style] || ",";
		const final = (options.allowReserved === true ? value : value.map((v) => encodeURIComponent(v))).join(joiner2);
		switch (options.style) {
			case "simple": return final;
			case "label": return `.${final}`;
			case "matrix": return `;${name}=${final}`;
			default: return `${name}=${final}`;
		}
	}
	const joiner = {
		simple: ",",
		label: ".",
		matrix: ";"
	}[options.style] || "&";
	const values = [];
	for (const v of value) if (options.style === "simple" || options.style === "label") values.push(options.allowReserved === true ? v : encodeURIComponent(v));
	else values.push(serializePrimitiveParam(name, v, options));
	return options.style === "label" || options.style === "matrix" ? `${joiner}${values.join(joiner)}` : values.join(joiner);
}
function createQuerySerializer(options) {
	return function querySerializer(queryParams) {
		const search = [];
		if (queryParams && typeof queryParams === "object") for (const name in queryParams) {
			const value = queryParams[name];
			if (value === void 0 || value === null) continue;
			if (Array.isArray(value)) {
				if (value.length === 0) continue;
				search.push(serializeArrayParam(name, value, {
					style: "form",
					explode: true,
					...options?.array,
					allowReserved: options?.allowReserved || false
				}));
				continue;
			}
			if (typeof value === "object") {
				search.push(serializeObjectParam(name, value, {
					style: "deepObject",
					explode: true,
					...options?.object,
					allowReserved: options?.allowReserved || false
				}));
				continue;
			}
			search.push(serializePrimitiveParam(name, value, options));
		}
		return search.join("&");
	};
}
function defaultPathSerializer(pathname, pathParams) {
	let nextURL = pathname;
	for (const match of pathname.match(PATH_PARAM_RE) ?? []) {
		let name = match.substring(1, match.length - 1);
		let explode = false;
		let style = "simple";
		if (name.endsWith("*")) {
			explode = true;
			name = name.substring(0, name.length - 1);
		}
		if (name.startsWith(".")) {
			style = "label";
			name = name.substring(1);
		} else if (name.startsWith(";")) {
			style = "matrix";
			name = name.substring(1);
		}
		if (!pathParams || pathParams[name] === void 0 || pathParams[name] === null) continue;
		const value = pathParams[name];
		if (Array.isArray(value)) {
			nextURL = nextURL.replace(match, serializeArrayParam(name, value, {
				style,
				explode
			}));
			continue;
		}
		if (typeof value === "object") {
			nextURL = nextURL.replace(match, serializeObjectParam(name, value, {
				style,
				explode
			}));
			continue;
		}
		if (style === "matrix") {
			nextURL = nextURL.replace(match, `;${serializePrimitiveParam(name, value)}`);
			continue;
		}
		nextURL = nextURL.replace(match, style === "label" ? `.${encodeURIComponent(value)}` : encodeURIComponent(value));
	}
	return nextURL;
}
function defaultBodySerializer(body, headers) {
	if (body instanceof FormData) return body;
	if (headers) {
		if ((headers.get instanceof Function ? headers.get("Content-Type") ?? headers.get("content-type") : headers["Content-Type"] ?? headers["content-type"]) === "application/x-www-form-urlencoded") return new URLSearchParams(body).toString();
	}
	return JSON.stringify(body);
}
function createFinalURL(pathname, options) {
	let finalURL = `${options.baseUrl}${pathname}`;
	if (options.params?.path) finalURL = options.pathSerializer(finalURL, options.params.path);
	let search = options.querySerializer(options.params.query ?? {});
	if (search.startsWith("?")) search = search.substring(1);
	if (search) finalURL += `?${search}`;
	return finalURL;
}
function mergeHeaders(...allHeaders) {
	const finalHeaders = new Headers();
	for (const h of allHeaders) {
		if (!h || typeof h !== "object") continue;
		const iterator = h instanceof Headers ? h.entries() : Object.entries(h);
		for (const [k, v] of iterator) if (v === null) finalHeaders.delete(k);
		else if (Array.isArray(v)) for (const v2 of v) finalHeaders.append(k, v2);
		else if (v !== void 0) finalHeaders.set(k, v);
	}
	return finalHeaders;
}
function removeTrailingSlash(url) {
	if (url.endsWith("/")) return url.substring(0, url.length - 1);
	return url;
}
//#endregion
//#region src/api/client.ts
function getCookie(name) {
	for (const cookie of document.cookie ? document.cookie.split(";") : []) {
		const [key, ...value] = cookie.trim().split("=");
		if (key === name) return decodeURIComponent(value.join("="));
	}
	return "";
}
var csrf = { onRequest({ request }) {
	if (![
		"GET",
		"HEAD",
		"OPTIONS",
		"TRACE"
	].includes(request.method)) request.headers.set("X-CSRFToken", getCookie("csrftoken"));
	return request;
} };
var api = createClient({
	baseUrl: globalThis.location?.origin ?? "",
	credentials: "same-origin",
	fetch: (request) => globalThis.fetch(request)
});
api.use(csrf);
require_react();
var import_jsx_runtime = require_jsx_runtime();
function Card({ title, actions, children }) {
	return /* @__PURE__ */ (0, import_jsx_runtime.jsxs)("section", {
		className: "p-4 mt-2 bg-base-100 rounded-box shadow-lg w-full",
		"aria-label": title,
		children: [/* @__PURE__ */ (0, import_jsx_runtime.jsxs)("div", {
			className: "flex items-center justify-between mb-2",
			children: [/* @__PURE__ */ (0, import_jsx_runtime.jsx)("h3", {
				className: "text-xl",
				children: title
			}), actions]
		}), children]
	});
}
function Row({ children }) {
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		className: "flex flex-wrap md:flex-nowrap gap-2",
		children
	});
}
function Alert({ level, children }) {
	const className = {
		info: "alert-info",
		warning: "alert-warning",
		error: "alert-error"
	}[level];
	return /* @__PURE__ */ (0, import_jsx_runtime.jsx)("div", {
		role: level === "error" ? "alert" : "status",
		className: `alert ${className} my-2`,
		children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)("span", { children })
	});
}
//#endregion
export { getCookie as a, api as i, Card as n, Row as r, Alert as t };
