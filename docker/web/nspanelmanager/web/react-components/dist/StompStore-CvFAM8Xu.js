import { i as __commonJSMin } from "./main-BWgDajaM.js";
import { t as create } from "./react-BrBB-CGW.js";
import { n as Notify, r as RemoveNotification } from "./NSPanelToastContainer-Abzvd6TK.js";
//#region node_modules/tslib/tslib.es6.mjs
/******************************************************************************
Copyright (c) Microsoft Corporation.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.
***************************************************************************** */
var extendStatics = function(d, b) {
	extendStatics = Object.setPrototypeOf || { __proto__: [] } instanceof Array && function(d, b) {
		d.__proto__ = b;
	} || function(d, b) {
		for (var p in b) if (Object.prototype.hasOwnProperty.call(b, p)) d[p] = b[p];
	};
	return extendStatics(d, b);
};
function __extends(d, b) {
	if (typeof b !== "function" && b !== null) throw new TypeError("Class extends value " + String(b) + " is not a constructor or null");
	extendStatics(d, b);
	function __() {
		this.constructor = d;
	}
	d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
}
function __awaiter(thisArg, _arguments, P, generator) {
	function adopt(value) {
		return value instanceof P ? value : new P(function(resolve) {
			resolve(value);
		});
	}
	return new (P || (P = Promise))(function(resolve, reject) {
		function fulfilled(value) {
			try {
				step(generator.next(value));
			} catch (e) {
				reject(e);
			}
		}
		function rejected(value) {
			try {
				step(generator["throw"](value));
			} catch (e) {
				reject(e);
			}
		}
		function step(result) {
			result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected);
		}
		step((generator = generator.apply(thisArg, _arguments || [])).next());
	});
}
function __generator(thisArg, body) {
	var _ = {
		label: 0,
		sent: function() {
			if (t[0] & 1) throw t[1];
			return t[1];
		},
		trys: [],
		ops: []
	}, f, y, t, g = Object.create((typeof Iterator === "function" ? Iterator : Object).prototype);
	return g.next = verb(0), g["throw"] = verb(1), g["return"] = verb(2), typeof Symbol === "function" && (g[Symbol.iterator] = function() {
		return this;
	}), g;
	function verb(n) {
		return function(v) {
			return step([n, v]);
		};
	}
	function step(op) {
		if (f) throw new TypeError("Generator is already executing.");
		while (g && (g = 0, op[0] && (_ = 0)), _) try {
			if (f = 1, y && (t = op[0] & 2 ? y["return"] : op[0] ? y["throw"] || ((t = y["return"]) && t.call(y), 0) : y.next) && !(t = t.call(y, op[1])).done) return t;
			if (y = 0, t) op = [op[0] & 2, t.value];
			switch (op[0]) {
				case 0:
				case 1:
					t = op;
					break;
				case 4:
					_.label++;
					return {
						value: op[1],
						done: false
					};
				case 5:
					_.label++;
					y = op[1];
					op = [0];
					continue;
				case 7:
					op = _.ops.pop();
					_.trys.pop();
					continue;
				default:
					if (!(t = _.trys, t = t.length > 0 && t[t.length - 1]) && (op[0] === 6 || op[0] === 2)) {
						_ = 0;
						continue;
					}
					if (op[0] === 3 && (!t || op[1] > t[0] && op[1] < t[3])) {
						_.label = op[1];
						break;
					}
					if (op[0] === 6 && _.label < t[1]) {
						_.label = t[1];
						t = op;
						break;
					}
					if (t && _.label < t[2]) {
						_.label = t[2];
						_.ops.push(op);
						break;
					}
					if (t[2]) _.ops.pop();
					_.trys.pop();
					continue;
			}
			op = body.call(thisArg, _);
		} catch (e) {
			op = [6, e];
			y = 0;
		} finally {
			f = t = 0;
		}
		if (op[0] & 5) throw op[1];
		return {
			value: op[0] ? op[1] : void 0,
			done: true
		};
	}
}
function __values(o) {
	var s = typeof Symbol === "function" && Symbol.iterator, m = s && o[s], i = 0;
	if (m) return m.call(o);
	if (o && typeof o.length === "number") return { next: function() {
		if (o && i >= o.length) o = void 0;
		return {
			value: o && o[i++],
			done: !o
		};
	} };
	throw new TypeError(s ? "Object is not iterable." : "Symbol.iterator is not defined.");
}
function __read(o, n) {
	var m = typeof Symbol === "function" && o[Symbol.iterator];
	if (!m) return o;
	var i = m.call(o), r, ar = [], e;
	try {
		while ((n === void 0 || n-- > 0) && !(r = i.next()).done) ar.push(r.value);
	} catch (error) {
		e = { error };
	} finally {
		try {
			if (r && !r.done && (m = i["return"])) m.call(i);
		} finally {
			if (e) throw e.error;
		}
	}
	return ar;
}
function __spreadArray(to, from, pack) {
	if (pack || arguments.length === 2) {
		for (var i = 0, l = from.length, ar; i < l; i++) if (ar || !(i in from)) {
			if (!ar) ar = Array.prototype.slice.call(from, 0, i);
			ar[i] = from[i];
		}
	}
	return to.concat(ar || Array.prototype.slice.call(from));
}
function __await(v) {
	return this instanceof __await ? (this.v = v, this) : new __await(v);
}
function __asyncGenerator(thisArg, _arguments, generator) {
	if (!Symbol.asyncIterator) throw new TypeError("Symbol.asyncIterator is not defined.");
	var g = generator.apply(thisArg, _arguments || []), i, q = [];
	return i = Object.create((typeof AsyncIterator === "function" ? AsyncIterator : Object).prototype), verb("next"), verb("throw"), verb("return", awaitReturn), i[Symbol.asyncIterator] = function() {
		return this;
	}, i;
	function awaitReturn(f) {
		return function(v) {
			return Promise.resolve(v).then(f, reject);
		};
	}
	function verb(n, f) {
		if (g[n]) {
			i[n] = function(v) {
				return new Promise(function(a, b) {
					q.push([
						n,
						v,
						a,
						b
					]) > 1 || resume(n, v);
				});
			};
			if (f) i[n] = f(i[n]);
		}
	}
	function resume(n, v) {
		try {
			step(g[n](v));
		} catch (e) {
			settle(q[0][3], e);
		}
	}
	function step(r) {
		r.value instanceof __await ? Promise.resolve(r.value.v).then(fulfill, reject) : settle(q[0][2], r);
	}
	function fulfill(value) {
		resume("next", value);
	}
	function reject(value) {
		resume("throw", value);
	}
	function settle(f, v) {
		if (f(v), q.shift(), q.length) resume(q[0][0], q[0][1]);
	}
}
function __asyncValues(o) {
	if (!Symbol.asyncIterator) throw new TypeError("Symbol.asyncIterator is not defined.");
	var m = o[Symbol.asyncIterator], i;
	return m ? m.call(o) : (o = typeof __values === "function" ? __values(o) : o[Symbol.iterator](), i = {}, verb("next"), verb("throw"), verb("return"), i[Symbol.asyncIterator] = function() {
		return this;
	}, i);
	function verb(n) {
		i[n] = o[n] && function(v) {
			return new Promise(function(resolve, reject) {
				v = o[n](v), settle(resolve, reject, v.done, v.value);
			});
		};
	}
	function settle(resolve, reject, d, v) {
		Promise.resolve(v).then(function(v) {
			resolve({
				value: v,
				done: d
			});
		}, reject);
	}
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/isFunction.js
function isFunction(value) {
	return typeof value === "function";
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/createErrorClass.js
function createErrorClass(createImpl) {
	var _super = function(instance) {
		Error.call(instance);
		instance.stack = (/* @__PURE__ */ new Error()).stack;
	};
	var ctorFunc = createImpl(_super);
	ctorFunc.prototype = Object.create(Error.prototype);
	ctorFunc.prototype.constructor = ctorFunc;
	return ctorFunc;
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/UnsubscriptionError.js
var UnsubscriptionError = createErrorClass(function(_super) {
	return function UnsubscriptionErrorImpl(errors) {
		_super(this);
		this.message = errors ? errors.length + " errors occurred during unsubscription:\n" + errors.map(function(err, i) {
			return i + 1 + ") " + err.toString();
		}).join("\n  ") : "";
		this.name = "UnsubscriptionError";
		this.errors = errors;
	};
});
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/arrRemove.js
function arrRemove(arr, item) {
	if (arr) {
		var index = arr.indexOf(item);
		0 <= index && arr.splice(index, 1);
	}
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/Subscription.js
var Subscription = function() {
	function Subscription(initialTeardown) {
		this.initialTeardown = initialTeardown;
		this.closed = false;
		this._parentage = null;
		this._finalizers = null;
	}
	Subscription.prototype.unsubscribe = function() {
		var e_1, _a, e_2, _b;
		var errors;
		if (!this.closed) {
			this.closed = true;
			var _parentage = this._parentage;
			if (_parentage) {
				this._parentage = null;
				if (Array.isArray(_parentage)) try {
					for (var _parentage_1 = __values(_parentage), _parentage_1_1 = _parentage_1.next(); !_parentage_1_1.done; _parentage_1_1 = _parentage_1.next()) _parentage_1_1.value.remove(this);
				} catch (e_1_1) {
					e_1 = { error: e_1_1 };
				} finally {
					try {
						if (_parentage_1_1 && !_parentage_1_1.done && (_a = _parentage_1.return)) _a.call(_parentage_1);
					} finally {
						if (e_1) throw e_1.error;
					}
				}
				else _parentage.remove(this);
			}
			var initialFinalizer = this.initialTeardown;
			if (isFunction(initialFinalizer)) try {
				initialFinalizer();
			} catch (e) {
				errors = e instanceof UnsubscriptionError ? e.errors : [e];
			}
			var _finalizers = this._finalizers;
			if (_finalizers) {
				this._finalizers = null;
				try {
					for (var _finalizers_1 = __values(_finalizers), _finalizers_1_1 = _finalizers_1.next(); !_finalizers_1_1.done; _finalizers_1_1 = _finalizers_1.next()) {
						var finalizer = _finalizers_1_1.value;
						try {
							execFinalizer(finalizer);
						} catch (err) {
							errors = errors !== null && errors !== void 0 ? errors : [];
							if (err instanceof UnsubscriptionError) errors = __spreadArray(__spreadArray([], __read(errors)), __read(err.errors));
							else errors.push(err);
						}
					}
				} catch (e_2_1) {
					e_2 = { error: e_2_1 };
				} finally {
					try {
						if (_finalizers_1_1 && !_finalizers_1_1.done && (_b = _finalizers_1.return)) _b.call(_finalizers_1);
					} finally {
						if (e_2) throw e_2.error;
					}
				}
			}
			if (errors) throw new UnsubscriptionError(errors);
		}
	};
	Subscription.prototype.add = function(teardown) {
		var _a;
		if (teardown && teardown !== this) {
			if (this.closed) execFinalizer(teardown);
			else {
				if (teardown instanceof Subscription) {
					if (teardown.closed || teardown._hasParent(this)) return;
					teardown._addParent(this);
				}
				(this._finalizers = (_a = this._finalizers) !== null && _a !== void 0 ? _a : []).push(teardown);
			}
		}
	};
	Subscription.prototype._hasParent = function(parent) {
		var _parentage = this._parentage;
		return _parentage === parent || Array.isArray(_parentage) && _parentage.includes(parent);
	};
	Subscription.prototype._addParent = function(parent) {
		var _parentage = this._parentage;
		this._parentage = Array.isArray(_parentage) ? (_parentage.push(parent), _parentage) : _parentage ? [_parentage, parent] : parent;
	};
	Subscription.prototype._removeParent = function(parent) {
		var _parentage = this._parentage;
		if (_parentage === parent) this._parentage = null;
		else if (Array.isArray(_parentage)) arrRemove(_parentage, parent);
	};
	Subscription.prototype.remove = function(teardown) {
		var _finalizers = this._finalizers;
		_finalizers && arrRemove(_finalizers, teardown);
		if (teardown instanceof Subscription) teardown._removeParent(this);
	};
	Subscription.EMPTY = (function() {
		var empty = new Subscription();
		empty.closed = true;
		return empty;
	})();
	return Subscription;
}();
var EMPTY_SUBSCRIPTION = Subscription.EMPTY;
function isSubscription(value) {
	return value instanceof Subscription || value && "closed" in value && isFunction(value.remove) && isFunction(value.add) && isFunction(value.unsubscribe);
}
function execFinalizer(finalizer) {
	if (isFunction(finalizer)) finalizer();
	else finalizer.unsubscribe();
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/config.js
var config = {
	onUnhandledError: null,
	onStoppedNotification: null,
	Promise: void 0,
	useDeprecatedSynchronousErrorHandling: false,
	useDeprecatedNextContext: false
};
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/scheduler/timeoutProvider.js
var timeoutProvider = {
	setTimeout: function(handler, timeout) {
		var args = [];
		for (var _i = 2; _i < arguments.length; _i++) args[_i - 2] = arguments[_i];
		var delegate = timeoutProvider.delegate;
		if (delegate === null || delegate === void 0 ? void 0 : delegate.setTimeout) return delegate.setTimeout.apply(delegate, __spreadArray([handler, timeout], __read(args)));
		return setTimeout.apply(void 0, __spreadArray([handler, timeout], __read(args)));
	},
	clearTimeout: function(handle) {
		var delegate = timeoutProvider.delegate;
		return ((delegate === null || delegate === void 0 ? void 0 : delegate.clearTimeout) || clearTimeout)(handle);
	},
	delegate: void 0
};
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/reportUnhandledError.js
function reportUnhandledError(err) {
	timeoutProvider.setTimeout(function() {
		var onUnhandledError = config.onUnhandledError;
		if (onUnhandledError) onUnhandledError(err);
		else throw err;
	});
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/noop.js
function noop() {}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/NotificationFactories.js
var COMPLETE_NOTIFICATION = (function() {
	return createNotification("C", void 0, void 0);
})();
function errorNotification(error) {
	return createNotification("E", void 0, error);
}
function nextNotification(value) {
	return createNotification("N", value, void 0);
}
function createNotification(kind, value, error) {
	return {
		kind,
		value,
		error
	};
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/errorContext.js
var context = null;
function errorContext(cb) {
	if (config.useDeprecatedSynchronousErrorHandling) {
		var isRoot = !context;
		if (isRoot) context = {
			errorThrown: false,
			error: null
		};
		cb();
		if (isRoot) {
			var _a = context, errorThrown = _a.errorThrown, error = _a.error;
			context = null;
			if (errorThrown) throw error;
		}
	} else cb();
}
function captureError(err) {
	if (config.useDeprecatedSynchronousErrorHandling && context) {
		context.errorThrown = true;
		context.error = err;
	}
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/Subscriber.js
var Subscriber = function(_super) {
	__extends(Subscriber, _super);
	function Subscriber(destination) {
		var _this = _super.call(this) || this;
		_this.isStopped = false;
		if (destination) {
			_this.destination = destination;
			if (isSubscription(destination)) destination.add(_this);
		} else _this.destination = EMPTY_OBSERVER;
		return _this;
	}
	Subscriber.create = function(next, error, complete) {
		return new SafeSubscriber(next, error, complete);
	};
	Subscriber.prototype.next = function(value) {
		if (this.isStopped) handleStoppedNotification(nextNotification(value), this);
		else this._next(value);
	};
	Subscriber.prototype.error = function(err) {
		if (this.isStopped) handleStoppedNotification(errorNotification(err), this);
		else {
			this.isStopped = true;
			this._error(err);
		}
	};
	Subscriber.prototype.complete = function() {
		if (this.isStopped) handleStoppedNotification(COMPLETE_NOTIFICATION, this);
		else {
			this.isStopped = true;
			this._complete();
		}
	};
	Subscriber.prototype.unsubscribe = function() {
		if (!this.closed) {
			this.isStopped = true;
			_super.prototype.unsubscribe.call(this);
			this.destination = null;
		}
	};
	Subscriber.prototype._next = function(value) {
		this.destination.next(value);
	};
	Subscriber.prototype._error = function(err) {
		try {
			this.destination.error(err);
		} finally {
			this.unsubscribe();
		}
	};
	Subscriber.prototype._complete = function() {
		try {
			this.destination.complete();
		} finally {
			this.unsubscribe();
		}
	};
	return Subscriber;
}(Subscription);
var _bind = Function.prototype.bind;
function bind(fn, thisArg) {
	return _bind.call(fn, thisArg);
}
var ConsumerObserver = function() {
	function ConsumerObserver(partialObserver) {
		this.partialObserver = partialObserver;
	}
	ConsumerObserver.prototype.next = function(value) {
		var partialObserver = this.partialObserver;
		if (partialObserver.next) try {
			partialObserver.next(value);
		} catch (error) {
			handleUnhandledError(error);
		}
	};
	ConsumerObserver.prototype.error = function(err) {
		var partialObserver = this.partialObserver;
		if (partialObserver.error) try {
			partialObserver.error(err);
		} catch (error) {
			handleUnhandledError(error);
		}
		else handleUnhandledError(err);
	};
	ConsumerObserver.prototype.complete = function() {
		var partialObserver = this.partialObserver;
		if (partialObserver.complete) try {
			partialObserver.complete();
		} catch (error) {
			handleUnhandledError(error);
		}
	};
	return ConsumerObserver;
}();
var SafeSubscriber = function(_super) {
	__extends(SafeSubscriber, _super);
	function SafeSubscriber(observerOrNext, error, complete) {
		var _this = _super.call(this) || this;
		var partialObserver;
		if (isFunction(observerOrNext) || !observerOrNext) partialObserver = {
			next: observerOrNext !== null && observerOrNext !== void 0 ? observerOrNext : void 0,
			error: error !== null && error !== void 0 ? error : void 0,
			complete: complete !== null && complete !== void 0 ? complete : void 0
		};
		else {
			var context_1;
			if (_this && config.useDeprecatedNextContext) {
				context_1 = Object.create(observerOrNext);
				context_1.unsubscribe = function() {
					return _this.unsubscribe();
				};
				partialObserver = {
					next: observerOrNext.next && bind(observerOrNext.next, context_1),
					error: observerOrNext.error && bind(observerOrNext.error, context_1),
					complete: observerOrNext.complete && bind(observerOrNext.complete, context_1)
				};
			} else partialObserver = observerOrNext;
		}
		_this.destination = new ConsumerObserver(partialObserver);
		return _this;
	}
	return SafeSubscriber;
}(Subscriber);
function handleUnhandledError(error) {
	if (config.useDeprecatedSynchronousErrorHandling) captureError(error);
	else reportUnhandledError(error);
}
function defaultErrorHandler(err) {
	throw err;
}
function handleStoppedNotification(notification, subscriber) {
	var onStoppedNotification = config.onStoppedNotification;
	onStoppedNotification && timeoutProvider.setTimeout(function() {
		return onStoppedNotification(notification, subscriber);
	});
}
var EMPTY_OBSERVER = {
	closed: true,
	next: noop,
	error: defaultErrorHandler,
	complete: noop
};
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/symbol/observable.js
var observable = (function() {
	return typeof Symbol === "function" && Symbol.observable || "@@observable";
})();
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/identity.js
function identity(x) {
	return x;
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/pipe.js
function pipeFromArray(fns) {
	if (fns.length === 0) return identity;
	if (fns.length === 1) return fns[0];
	return function piped(input) {
		return fns.reduce(function(prev, fn) {
			return fn(prev);
		}, input);
	};
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/Observable.js
var Observable = function() {
	function Observable(subscribe) {
		if (subscribe) this._subscribe = subscribe;
	}
	Observable.prototype.lift = function(operator) {
		var observable = new Observable();
		observable.source = this;
		observable.operator = operator;
		return observable;
	};
	Observable.prototype.subscribe = function(observerOrNext, error, complete) {
		var _this = this;
		var subscriber = isSubscriber(observerOrNext) ? observerOrNext : new SafeSubscriber(observerOrNext, error, complete);
		errorContext(function() {
			var _a = _this, operator = _a.operator, source = _a.source;
			subscriber.add(operator ? operator.call(subscriber, source) : source ? _this._subscribe(subscriber) : _this._trySubscribe(subscriber));
		});
		return subscriber;
	};
	Observable.prototype._trySubscribe = function(sink) {
		try {
			return this._subscribe(sink);
		} catch (err) {
			sink.error(err);
		}
	};
	Observable.prototype.forEach = function(next, promiseCtor) {
		var _this = this;
		promiseCtor = getPromiseCtor(promiseCtor);
		return new promiseCtor(function(resolve, reject) {
			var subscriber = new SafeSubscriber({
				next: function(value) {
					try {
						next(value);
					} catch (err) {
						reject(err);
						subscriber.unsubscribe();
					}
				},
				error: reject,
				complete: resolve
			});
			_this.subscribe(subscriber);
		});
	};
	Observable.prototype._subscribe = function(subscriber) {
		var _a;
		return (_a = this.source) === null || _a === void 0 ? void 0 : _a.subscribe(subscriber);
	};
	Observable.prototype[observable] = function() {
		return this;
	};
	Observable.prototype.pipe = function() {
		var operations = [];
		for (var _i = 0; _i < arguments.length; _i++) operations[_i] = arguments[_i];
		return pipeFromArray(operations)(this);
	};
	Observable.prototype.toPromise = function(promiseCtor) {
		var _this = this;
		promiseCtor = getPromiseCtor(promiseCtor);
		return new promiseCtor(function(resolve, reject) {
			var value;
			_this.subscribe(function(x) {
				return value = x;
			}, function(err) {
				return reject(err);
			}, function() {
				return resolve(value);
			});
		});
	};
	Observable.create = function(subscribe) {
		return new Observable(subscribe);
	};
	return Observable;
}();
function getPromiseCtor(promiseCtor) {
	var _a;
	return (_a = promiseCtor !== null && promiseCtor !== void 0 ? promiseCtor : config.Promise) !== null && _a !== void 0 ? _a : Promise;
}
function isObserver(value) {
	return value && isFunction(value.next) && isFunction(value.error) && isFunction(value.complete);
}
function isSubscriber(value) {
	return value && value instanceof Subscriber || isObserver(value) && isSubscription(value);
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/lift.js
function hasLift(source) {
	return isFunction(source === null || source === void 0 ? void 0 : source.lift);
}
function operate(init) {
	return function(source) {
		if (hasLift(source)) return source.lift(function(liftedSource) {
			try {
				return init(liftedSource, this);
			} catch (err) {
				this.error(err);
			}
		});
		throw new TypeError("Unable to lift unknown Observable type");
	};
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/operators/OperatorSubscriber.js
function createOperatorSubscriber(destination, onNext, onComplete, onError, onFinalize) {
	return new OperatorSubscriber(destination, onNext, onComplete, onError, onFinalize);
}
var OperatorSubscriber = function(_super) {
	__extends(OperatorSubscriber, _super);
	function OperatorSubscriber(destination, onNext, onComplete, onError, onFinalize, shouldUnsubscribe) {
		var _this = _super.call(this, destination) || this;
		_this.onFinalize = onFinalize;
		_this.shouldUnsubscribe = shouldUnsubscribe;
		_this._next = onNext ? function(value) {
			try {
				onNext(value);
			} catch (err) {
				destination.error(err);
			}
		} : _super.prototype._next;
		_this._error = onError ? function(err) {
			try {
				onError(err);
			} catch (err) {
				destination.error(err);
			} finally {
				this.unsubscribe();
			}
		} : _super.prototype._error;
		_this._complete = onComplete ? function() {
			try {
				onComplete();
			} catch (err) {
				destination.error(err);
			} finally {
				this.unsubscribe();
			}
		} : _super.prototype._complete;
		return _this;
	}
	OperatorSubscriber.prototype.unsubscribe = function() {
		var _a;
		if (!this.shouldUnsubscribe || this.shouldUnsubscribe()) {
			var closed_1 = this.closed;
			_super.prototype.unsubscribe.call(this);
			!closed_1 && ((_a = this.onFinalize) === null || _a === void 0 || _a.call(this));
		}
	};
	return OperatorSubscriber;
}(Subscriber);
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/ObjectUnsubscribedError.js
var ObjectUnsubscribedError = createErrorClass(function(_super) {
	return function ObjectUnsubscribedErrorImpl() {
		_super(this);
		this.name = "ObjectUnsubscribedError";
		this.message = "object unsubscribed";
	};
});
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/Subject.js
var Subject = function(_super) {
	__extends(Subject, _super);
	function Subject() {
		var _this = _super.call(this) || this;
		_this.closed = false;
		_this.currentObservers = null;
		_this.observers = [];
		_this.isStopped = false;
		_this.hasError = false;
		_this.thrownError = null;
		return _this;
	}
	Subject.prototype.lift = function(operator) {
		var subject = new AnonymousSubject(this, this);
		subject.operator = operator;
		return subject;
	};
	Subject.prototype._throwIfClosed = function() {
		if (this.closed) throw new ObjectUnsubscribedError();
	};
	Subject.prototype.next = function(value) {
		var _this = this;
		errorContext(function() {
			var e_1, _a;
			_this._throwIfClosed();
			if (!_this.isStopped) {
				if (!_this.currentObservers) _this.currentObservers = Array.from(_this.observers);
				try {
					for (var _b = __values(_this.currentObservers), _c = _b.next(); !_c.done; _c = _b.next()) _c.value.next(value);
				} catch (e_1_1) {
					e_1 = { error: e_1_1 };
				} finally {
					try {
						if (_c && !_c.done && (_a = _b.return)) _a.call(_b);
					} finally {
						if (e_1) throw e_1.error;
					}
				}
			}
		});
	};
	Subject.prototype.error = function(err) {
		var _this = this;
		errorContext(function() {
			_this._throwIfClosed();
			if (!_this.isStopped) {
				_this.hasError = _this.isStopped = true;
				_this.thrownError = err;
				var observers = _this.observers;
				while (observers.length) observers.shift().error(err);
			}
		});
	};
	Subject.prototype.complete = function() {
		var _this = this;
		errorContext(function() {
			_this._throwIfClosed();
			if (!_this.isStopped) {
				_this.isStopped = true;
				var observers = _this.observers;
				while (observers.length) observers.shift().complete();
			}
		});
	};
	Subject.prototype.unsubscribe = function() {
		this.isStopped = this.closed = true;
		this.observers = this.currentObservers = null;
	};
	Object.defineProperty(Subject.prototype, "observed", {
		get: function() {
			var _a;
			return ((_a = this.observers) === null || _a === void 0 ? void 0 : _a.length) > 0;
		},
		enumerable: false,
		configurable: true
	});
	Subject.prototype._trySubscribe = function(subscriber) {
		this._throwIfClosed();
		return _super.prototype._trySubscribe.call(this, subscriber);
	};
	Subject.prototype._subscribe = function(subscriber) {
		this._throwIfClosed();
		this._checkFinalizedStatuses(subscriber);
		return this._innerSubscribe(subscriber);
	};
	Subject.prototype._innerSubscribe = function(subscriber) {
		var _this = this;
		var _a = this, hasError = _a.hasError, isStopped = _a.isStopped, observers = _a.observers;
		if (hasError || isStopped) return EMPTY_SUBSCRIPTION;
		this.currentObservers = null;
		observers.push(subscriber);
		return new Subscription(function() {
			_this.currentObservers = null;
			arrRemove(observers, subscriber);
		});
	};
	Subject.prototype._checkFinalizedStatuses = function(subscriber) {
		var _a = this, hasError = _a.hasError, thrownError = _a.thrownError, isStopped = _a.isStopped;
		if (hasError) subscriber.error(thrownError);
		else if (isStopped) subscriber.complete();
	};
	Subject.prototype.asObservable = function() {
		var observable = new Observable();
		observable.source = this;
		return observable;
	};
	Subject.create = function(destination, source) {
		return new AnonymousSubject(destination, source);
	};
	return Subject;
}(Observable);
var AnonymousSubject = function(_super) {
	__extends(AnonymousSubject, _super);
	function AnonymousSubject(destination, source) {
		var _this = _super.call(this) || this;
		_this.destination = destination;
		_this.source = source;
		return _this;
	}
	AnonymousSubject.prototype.next = function(value) {
		var _a, _b;
		(_b = (_a = this.destination) === null || _a === void 0 ? void 0 : _a.next) === null || _b === void 0 || _b.call(_a, value);
	};
	AnonymousSubject.prototype.error = function(err) {
		var _a, _b;
		(_b = (_a = this.destination) === null || _a === void 0 ? void 0 : _a.error) === null || _b === void 0 || _b.call(_a, err);
	};
	AnonymousSubject.prototype.complete = function() {
		var _a, _b;
		(_b = (_a = this.destination) === null || _a === void 0 ? void 0 : _a.complete) === null || _b === void 0 || _b.call(_a);
	};
	AnonymousSubject.prototype._subscribe = function(subscriber) {
		var _a, _b;
		return (_b = (_a = this.source) === null || _a === void 0 ? void 0 : _a.subscribe(subscriber)) !== null && _b !== void 0 ? _b : EMPTY_SUBSCRIPTION;
	};
	return AnonymousSubject;
}(Subject);
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/BehaviorSubject.js
var BehaviorSubject = function(_super) {
	__extends(BehaviorSubject, _super);
	function BehaviorSubject(_value) {
		var _this = _super.call(this) || this;
		_this._value = _value;
		return _this;
	}
	Object.defineProperty(BehaviorSubject.prototype, "value", {
		get: function() {
			return this.getValue();
		},
		enumerable: false,
		configurable: true
	});
	BehaviorSubject.prototype._subscribe = function(subscriber) {
		var subscription = _super.prototype._subscribe.call(this, subscriber);
		!subscription.closed && subscriber.next(this._value);
		return subscription;
	};
	BehaviorSubject.prototype.getValue = function() {
		var _a = this, hasError = _a.hasError, thrownError = _a.thrownError, _value = _a._value;
		if (hasError) throw thrownError;
		this._throwIfClosed();
		return _value;
	};
	BehaviorSubject.prototype.next = function(value) {
		_super.prototype.next.call(this, this._value = value);
	};
	return BehaviorSubject;
}(Subject);
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/observable/empty.js
var EMPTY = new Observable(function(subscriber) {
	return subscriber.complete();
});
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/isArrayLike.js
var isArrayLike = (function(x) {
	return x && typeof x.length === "number" && typeof x !== "function";
});
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/isPromise.js
function isPromise(value) {
	return isFunction(value === null || value === void 0 ? void 0 : value.then);
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/isInteropObservable.js
function isInteropObservable(input) {
	return isFunction(input[observable]);
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/isAsyncIterable.js
function isAsyncIterable(obj) {
	return Symbol.asyncIterator && isFunction(obj === null || obj === void 0 ? void 0 : obj[Symbol.asyncIterator]);
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/throwUnobservableError.js
function createInvalidObservableTypeError(input) {
	return /* @__PURE__ */ new TypeError("You provided " + (input !== null && typeof input === "object" ? "an invalid object" : "'" + input + "'") + " where a stream was expected. You can provide an Observable, Promise, ReadableStream, Array, AsyncIterable, or Iterable.");
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/symbol/iterator.js
function getSymbolIterator() {
	if (typeof Symbol !== "function" || !Symbol.iterator) return "@@iterator";
	return Symbol.iterator;
}
var iterator = getSymbolIterator();
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/isIterable.js
function isIterable(input) {
	return isFunction(input === null || input === void 0 ? void 0 : input[iterator]);
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/isReadableStreamLike.js
function readableStreamLikeToAsyncGenerator(readableStream) {
	return __asyncGenerator(this, arguments, function readableStreamLikeToAsyncGenerator_1() {
		var reader, _a, value, done;
		return __generator(this, function(_b) {
			switch (_b.label) {
				case 0:
					reader = readableStream.getReader();
					_b.label = 1;
				case 1:
					_b.trys.push([
						1,
						,
						9,
						10
					]);
					_b.label = 2;
				case 2: return [4, __await(reader.read())];
				case 3:
					_a = _b.sent(), value = _a.value, done = _a.done;
					if (!done) return [3, 5];
					return [4, __await(void 0)];
				case 4: return [2, _b.sent()];
				case 5: return [4, __await(value)];
				case 6: return [4, _b.sent()];
				case 7:
					_b.sent();
					return [3, 2];
				case 8: return [3, 10];
				case 9:
					reader.releaseLock();
					return [7];
				case 10: return [2];
			}
		});
	});
}
function isReadableStreamLike(obj) {
	return isFunction(obj === null || obj === void 0 ? void 0 : obj.getReader);
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/observable/innerFrom.js
function innerFrom(input) {
	if (input instanceof Observable) return input;
	if (input != null) {
		if (isInteropObservable(input)) return fromInteropObservable(input);
		if (isArrayLike(input)) return fromArrayLike(input);
		if (isPromise(input)) return fromPromise(input);
		if (isAsyncIterable(input)) return fromAsyncIterable(input);
		if (isIterable(input)) return fromIterable(input);
		if (isReadableStreamLike(input)) return fromReadableStreamLike(input);
	}
	throw createInvalidObservableTypeError(input);
}
function fromInteropObservable(obj) {
	return new Observable(function(subscriber) {
		var obs = obj[observable]();
		if (isFunction(obs.subscribe)) return obs.subscribe(subscriber);
		throw new TypeError("Provided object does not correctly implement Symbol.observable");
	});
}
function fromArrayLike(array) {
	return new Observable(function(subscriber) {
		for (var i = 0; i < array.length && !subscriber.closed; i++) subscriber.next(array[i]);
		subscriber.complete();
	});
}
function fromPromise(promise) {
	return new Observable(function(subscriber) {
		promise.then(function(value) {
			if (!subscriber.closed) {
				subscriber.next(value);
				subscriber.complete();
			}
		}, function(err) {
			return subscriber.error(err);
		}).then(null, reportUnhandledError);
	});
}
function fromIterable(iterable) {
	return new Observable(function(subscriber) {
		var e_1, _a;
		try {
			for (var iterable_1 = __values(iterable), iterable_1_1 = iterable_1.next(); !iterable_1_1.done; iterable_1_1 = iterable_1.next()) {
				var value = iterable_1_1.value;
				subscriber.next(value);
				if (subscriber.closed) return;
			}
		} catch (e_1_1) {
			e_1 = { error: e_1_1 };
		} finally {
			try {
				if (iterable_1_1 && !iterable_1_1.done && (_a = iterable_1.return)) _a.call(iterable_1);
			} finally {
				if (e_1) throw e_1.error;
			}
		}
		subscriber.complete();
	});
}
function fromAsyncIterable(asyncIterable) {
	return new Observable(function(subscriber) {
		process(asyncIterable, subscriber).catch(function(err) {
			return subscriber.error(err);
		});
	});
}
function fromReadableStreamLike(readableStream) {
	return fromAsyncIterable(readableStreamLikeToAsyncGenerator(readableStream));
}
function process(asyncIterable, subscriber) {
	var asyncIterable_1, asyncIterable_1_1;
	var e_2, _a;
	return __awaiter(this, void 0, void 0, function() {
		var value, e_2_1;
		return __generator(this, function(_b) {
			switch (_b.label) {
				case 0:
					_b.trys.push([
						0,
						5,
						6,
						11
					]);
					asyncIterable_1 = __asyncValues(asyncIterable);
					_b.label = 1;
				case 1: return [4, asyncIterable_1.next()];
				case 2:
					if (!(asyncIterable_1_1 = _b.sent(), !asyncIterable_1_1.done)) return [3, 4];
					value = asyncIterable_1_1.value;
					subscriber.next(value);
					if (subscriber.closed) return [2];
					_b.label = 3;
				case 3: return [3, 1];
				case 4: return [3, 11];
				case 5:
					e_2_1 = _b.sent();
					e_2 = { error: e_2_1 };
					return [3, 11];
				case 6:
					_b.trys.push([
						6,
						,
						9,
						10
					]);
					if (!(asyncIterable_1_1 && !asyncIterable_1_1.done && (_a = asyncIterable_1.return))) return [3, 8];
					return [4, _a.call(asyncIterable_1)];
				case 7:
					_b.sent();
					_b.label = 8;
				case 8: return [3, 10];
				case 9:
					if (e_2) throw e_2.error;
					return [7];
				case 10: return [7];
				case 11:
					subscriber.complete();
					return [2];
			}
		});
	});
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/util/EmptyError.js
var EmptyError = createErrorClass(function(_super) {
	return function EmptyErrorImpl() {
		_super(this);
		this.name = "EmptyError";
		this.message = "no elements in sequence";
	};
});
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/firstValueFrom.js
function firstValueFrom(source, config) {
	var hasConfig = typeof config === "object";
	return new Promise(function(resolve, reject) {
		var subscriber = new SafeSubscriber({
			next: function(value) {
				resolve(value);
				subscriber.unsubscribe();
			},
			error: reject,
			complete: function() {
				if (hasConfig) resolve(config.defaultValue);
				else reject(new EmptyError());
			}
		});
		source.subscribe(subscriber);
	});
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/operators/filter.js
function filter(predicate, thisArg) {
	return operate(function(source, subscriber) {
		var index = 0;
		source.subscribe(createOperatorSubscriber(subscriber, function(value) {
			return predicate.call(thisArg, value, index++) && subscriber.next(value);
		}));
	});
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/operators/take.js
function take(count) {
	return count <= 0 ? function() {
		return EMPTY;
	} : operate(function(source, subscriber) {
		var seen = 0;
		source.subscribe(createOperatorSubscriber(subscriber, function(value) {
			if (++seen <= count) {
				subscriber.next(value);
				if (count <= seen) subscriber.complete();
			}
		}));
	});
}
//#endregion
//#region node_modules/rxjs/dist/esm5/internal/operators/share.js
function share(options) {
	if (options === void 0) options = {};
	var _a = options.connector, connector = _a === void 0 ? function() {
		return new Subject();
	} : _a, _b = options.resetOnError, resetOnError = _b === void 0 ? true : _b, _c = options.resetOnComplete, resetOnComplete = _c === void 0 ? true : _c, _d = options.resetOnRefCountZero, resetOnRefCountZero = _d === void 0 ? true : _d;
	return function(wrapperSource) {
		var connection;
		var resetConnection;
		var subject;
		var refCount = 0;
		var hasCompleted = false;
		var hasErrored = false;
		var cancelReset = function() {
			resetConnection === null || resetConnection === void 0 || resetConnection.unsubscribe();
			resetConnection = void 0;
		};
		var reset = function() {
			cancelReset();
			connection = subject = void 0;
			hasCompleted = hasErrored = false;
		};
		var resetAndUnsubscribe = function() {
			var conn = connection;
			reset();
			conn === null || conn === void 0 || conn.unsubscribe();
		};
		return operate(function(source, subscriber) {
			refCount++;
			if (!hasErrored && !hasCompleted) cancelReset();
			var dest = subject = subject !== null && subject !== void 0 ? subject : connector();
			subscriber.add(function() {
				refCount--;
				if (refCount === 0 && !hasErrored && !hasCompleted) resetConnection = handleReset(resetAndUnsubscribe, resetOnRefCountZero);
			});
			dest.subscribe(subscriber);
			if (!connection && refCount > 0) {
				connection = new SafeSubscriber({
					next: function(value) {
						return dest.next(value);
					},
					error: function(err) {
						hasErrored = true;
						cancelReset();
						resetConnection = handleReset(reset, resetOnError, err);
						dest.error(err);
					},
					complete: function() {
						hasCompleted = true;
						cancelReset();
						resetConnection = handleReset(reset, resetOnComplete);
						dest.complete();
					}
				});
				innerFrom(source).subscribe(connection);
			}
		})(wrapperSource);
	};
}
function handleReset(reset, on) {
	var args = [];
	for (var _i = 2; _i < arguments.length; _i++) args[_i - 2] = arguments[_i];
	if (on === true) {
		reset();
		return;
	}
	if (on === false) return;
	var onSubscriber = new SafeSubscriber({ next: function() {
		onSubscriber.unsubscribe();
		reset();
	} });
	return innerFrom(on.apply(void 0, __spreadArray([], __read(args)))).subscribe(onSubscriber);
}
//#endregion
//#region node_modules/@stomp/rx-stomp/esm6/rx-stomp-state.js
var import_stomp_umd = (/* @__PURE__ */ __commonJSMin(((exports, module) => {
	(function(global, factory) {
		typeof exports === "object" && typeof module !== "undefined" ? factory(exports) : typeof define === "function" && define.amd ? define(["exports"], factory) : (global = typeof globalThis !== "undefined" ? globalThis : global || self, factory(global.StompJs = {}));
	})(exports, (function(exports$1) {
		"use strict";
		/**
		* @internal
		*/
		function augmentWebsocket(webSocket, debug) {
			webSocket.terminate = function() {
				const noOp = () => {};
				this.onerror = noOp;
				this.onmessage = noOp;
				this.onopen = noOp;
				const ts = /* @__PURE__ */ new Date();
				const id = Math.random().toString().substring(2, 8);
				const origOnClose = this.onclose;
				this.onclose = (closeEvent) => {
					const delay = (/* @__PURE__ */ new Date()).getTime() - ts.getTime();
					debug(`Discarded socket (#${id})  closed after ${delay}ms, with code/reason: ${closeEvent.code}/${closeEvent.reason}`);
				};
				this.close();
				origOnClose?.call(webSocket, {
					code: 4001,
					reason: `Quick discarding socket (#${id}) without waiting for the shutdown sequence.`,
					wasClean: false
				});
			};
		}
		/**
		* Some byte values, used as per STOMP specifications.
		*
		* Part of `@stomp/stompjs`.
		*
		* @internal
		*/
		const BYTE = {
			LF: "\n",
			NULL: "\0"
		};
		/**
		* Frame class represents a STOMP frame.
		*
		* @internal
		*/
		class FrameImpl {
			/**
			* body of the frame
			*/
			get body() {
				if (!this._body && this.isBinaryBody) this._body = new TextDecoder().decode(this._binaryBody);
				return this._body || "";
			}
			/**
			* body as Uint8Array
			*/
			get binaryBody() {
				if (!this._binaryBody && !this.isBinaryBody) this._binaryBody = new TextEncoder().encode(this._body);
				return this._binaryBody;
			}
			/**
			* Frame constructor. `command`, `headers` and `body` are available as properties.
			*
			* @internal
			*/
			constructor(params) {
				const { command, headers, body, binaryBody, escapeHeaderValues, skipContentLengthHeader } = params;
				this.command = command;
				this.headers = Object.assign({}, headers || {});
				if (binaryBody) {
					this._binaryBody = binaryBody;
					this.isBinaryBody = true;
				} else {
					this._body = body || "";
					this.isBinaryBody = false;
				}
				this.escapeHeaderValues = escapeHeaderValues || false;
				this.skipContentLengthHeader = skipContentLengthHeader || false;
			}
			/**
			* deserialize a STOMP Frame from raw data.
			*
			* @internal
			*/
			static fromRawFrame(rawFrame, escapeHeaderValues) {
				const headers = {};
				const trim = (str) => str.replace(/^\s+|\s+$/g, "");
				for (const header of rawFrame.headers.reverse()) {
					header.indexOf(":");
					const key = trim(header[0]);
					let value = trim(header[1]);
					if (escapeHeaderValues && rawFrame.command !== "CONNECT" && rawFrame.command !== "CONNECTED") value = FrameImpl.hdrValueUnEscape(value);
					headers[key] = value;
				}
				return new FrameImpl({
					command: rawFrame.command,
					headers,
					binaryBody: rawFrame.binaryBody,
					escapeHeaderValues
				});
			}
			/**
			* @internal
			*/
			toString() {
				return this.serializeCmdAndHeaders();
			}
			/**
			* serialize this Frame in a format suitable to be passed to WebSocket.
			* If the body is string the output will be string.
			* If the body is binary (i.e. of type Unit8Array) it will be serialized to ArrayBuffer.
			*
			* @internal
			*/
			serialize() {
				const cmdAndHeaders = this.serializeCmdAndHeaders();
				if (this.isBinaryBody) return FrameImpl.toUnit8Array(cmdAndHeaders, this._binaryBody).buffer;
				else return cmdAndHeaders + this._body + BYTE.NULL;
			}
			serializeCmdAndHeaders() {
				const lines = [this.command];
				if (this.skipContentLengthHeader) delete this.headers["content-length"];
				for (const name of Object.keys(this.headers || {})) {
					const value = this.headers[name];
					if (this.escapeHeaderValues && this.command !== "CONNECT" && this.command !== "CONNECTED") lines.push(`${name}:${FrameImpl.hdrValueEscape(`${value}`)}`);
					else lines.push(`${name}:${value}`);
				}
				if (this.isBinaryBody || !this.isBodyEmpty() && !this.skipContentLengthHeader) lines.push(`content-length:${this.bodyLength()}`);
				return lines.join(BYTE.LF) + BYTE.LF + BYTE.LF;
			}
			isBodyEmpty() {
				return this.bodyLength() === 0;
			}
			bodyLength() {
				const binaryBody = this.binaryBody;
				return binaryBody ? binaryBody.length : 0;
			}
			/**
			* Compute the size of a UTF-8 string by counting its number of bytes
			* (and not the number of characters composing the string)
			*/
			static sizeOfUTF8(s) {
				return s ? new TextEncoder().encode(s).length : 0;
			}
			static toUnit8Array(cmdAndHeaders, binaryBody) {
				const uint8CmdAndHeaders = new TextEncoder().encode(cmdAndHeaders);
				const nullTerminator = new Uint8Array([0]);
				const uint8Frame = new Uint8Array(uint8CmdAndHeaders.length + binaryBody.length + nullTerminator.length);
				uint8Frame.set(uint8CmdAndHeaders);
				uint8Frame.set(binaryBody, uint8CmdAndHeaders.length);
				uint8Frame.set(nullTerminator, uint8CmdAndHeaders.length + binaryBody.length);
				return uint8Frame;
			}
			/**
			* Serialize a STOMP frame as per STOMP standards, suitable to be sent to the STOMP broker.
			*
			* @internal
			*/
			static marshall(params) {
				return new FrameImpl(params).serialize();
			}
			/**
			*  Escape header values
			*/
			static hdrValueEscape(str) {
				return str.replace(/\\/g, "\\\\").replace(/\r/g, "\\r").replace(/\n/g, "\\n").replace(/:/g, "\\c");
			}
			/**
			* UnEscape header values
			*/
			static hdrValueUnEscape(str) {
				return str.replace(/\\r/g, "\r").replace(/\\n/g, "\n").replace(/\\c/g, ":").replace(/\\\\/g, "\\");
			}
		}
		/**
		* @internal
		*/
		const NULL = 0;
		/**
		* @internal
		*/
		const LF = 10;
		/**
		* @internal
		*/
		const CR = 13;
		/**
		* @internal
		*/
		const COLON = 58;
		/**
		* This is an evented, rec descent parser.
		* A stream of Octets can be passed and whenever it recognizes
		* a complete Frame or an incoming ping it will invoke the registered callbacks.
		*
		* All incoming Octets are fed into _onByte function.
		* Depending on current state the _onByte function keeps changing.
		* Depending on the state it keeps accumulating into _token and _results.
		* State is indicated by current value of _onByte, all states are named as _collect.
		*
		* STOMP standards https://stomp.github.io/stomp-specification-1.2.html
		* imply that all lengths are considered in bytes (instead of string lengths).
		* So, before actual parsing, if the incoming data is String it is converted to Octets.
		* This allows faithful implementation of the protocol and allows NULL Octets to be present in the body.
		*
		* There is no peek function on the incoming data.
		* When a state change occurs based on an Octet without consuming the Octet,
		* the Octet, after state change, is fed again (_reinjectByte).
		* This became possible as the state change can be determined by inspecting just one Octet.
		*
		* There are two modes to collect the body, if content-length header is there then it by counting Octets
		* otherwise it is determined by NULL terminator.
		*
		* Following the standards, the command and headers are converted to Strings
		* and the body is returned as Octets.
		* Headers are returned as an array and not as Hash - to allow multiple occurrence of an header.
		*
		* This parser does not use Regular Expressions as that can only operate on Strings.
		*
		* It handles if multiple STOMP frames are given as one chunk, a frame is split into multiple chunks, or
		* any combination there of. The parser remembers its state (any partial frame) and continues when a new chunk
		* is pushed.
		*
		* Typically the higher level function will convert headers to Hash, handle unescaping of header values
		* (which is protocol version specific), and convert body to text.
		*
		* Check the parser.spec.js to understand cases that this parser is supposed to handle.
		*
		* Part of `@stomp/stompjs`.
		*
		* @internal
		*/
		class Parser {
			constructor(onFrame, onIncomingPing) {
				this.onFrame = onFrame;
				this.onIncomingPing = onIncomingPing;
				this._encoder = new TextEncoder();
				this._decoder = new TextDecoder();
				this._token = [];
				this._initState();
			}
			parseChunk(segment, appendMissingNULLonIncoming = false) {
				let chunk;
				if (typeof segment === "string") chunk = this._encoder.encode(segment);
				else chunk = new Uint8Array(segment);
				if (appendMissingNULLonIncoming && chunk[chunk.length - 1] !== 0) {
					const chunkWithNull = new Uint8Array(chunk.length + 1);
					chunkWithNull.set(chunk, 0);
					chunkWithNull[chunk.length] = 0;
					chunk = chunkWithNull;
				}
				for (let i = 0; i < chunk.length; i++) {
					const byte = chunk[i];
					this._onByte(byte);
				}
			}
			_collectFrame(byte) {
				if (byte === NULL) return;
				if (byte === CR) return;
				if (byte === LF) {
					this.onIncomingPing();
					return;
				}
				this._onByte = this._collectCommand;
				this._reinjectByte(byte);
			}
			_collectCommand(byte) {
				if (byte === CR) return;
				if (byte === LF) {
					this._results.command = this._consumeTokenAsUTF8();
					this._onByte = this._collectHeaders;
					return;
				}
				this._consumeByte(byte);
			}
			_collectHeaders(byte) {
				if (byte === CR) return;
				if (byte === LF) {
					this._setupCollectBody();
					return;
				}
				this._onByte = this._collectHeaderKey;
				this._reinjectByte(byte);
			}
			_reinjectByte(byte) {
				this._onByte(byte);
			}
			_collectHeaderKey(byte) {
				if (byte === COLON) {
					this._headerKey = this._consumeTokenAsUTF8();
					this._onByte = this._collectHeaderValue;
					return;
				}
				this._consumeByte(byte);
			}
			_collectHeaderValue(byte) {
				if (byte === CR) return;
				if (byte === LF) {
					this._results.headers.push([this._headerKey, this._consumeTokenAsUTF8()]);
					this._headerKey = void 0;
					this._onByte = this._collectHeaders;
					return;
				}
				this._consumeByte(byte);
			}
			_setupCollectBody() {
				const contentLengthHeader = this._results.headers.filter((header) => {
					return header[0] === "content-length";
				})[0];
				if (contentLengthHeader) {
					this._bodyBytesRemaining = parseInt(contentLengthHeader[1], 10);
					this._onByte = this._collectBodyFixedSize;
				} else this._onByte = this._collectBodyNullTerminated;
			}
			_collectBodyNullTerminated(byte) {
				if (byte === NULL) {
					this._retrievedBody();
					return;
				}
				this._consumeByte(byte);
			}
			_collectBodyFixedSize(byte) {
				if (this._bodyBytesRemaining-- === 0) {
					this._retrievedBody();
					return;
				}
				this._consumeByte(byte);
			}
			_retrievedBody() {
				this._results.binaryBody = this._consumeTokenAsRaw();
				try {
					this.onFrame(this._results);
				} catch (e) {
					console.log(`Ignoring an exception thrown by a frame handler. Original exception: `, e);
				}
				this._initState();
			}
			_consumeByte(byte) {
				this._token.push(byte);
			}
			_consumeTokenAsUTF8() {
				return this._decoder.decode(this._consumeTokenAsRaw());
			}
			_consumeTokenAsRaw() {
				const rawResult = new Uint8Array(this._token);
				this._token = [];
				return rawResult;
			}
			_initState() {
				this._results = {
					command: void 0,
					headers: [],
					binaryBody: void 0
				};
				this._token = [];
				this._headerKey = void 0;
				this._onByte = this._collectFrame;
			}
		}
		/**
		* Possible states for the IStompSocket
		*/
		exports$1.StompSocketState = void 0;
		(function(StompSocketState) {
			StompSocketState[StompSocketState["CONNECTING"] = 0] = "CONNECTING";
			StompSocketState[StompSocketState["OPEN"] = 1] = "OPEN";
			StompSocketState[StompSocketState["CLOSING"] = 2] = "CLOSING";
			StompSocketState[StompSocketState["CLOSED"] = 3] = "CLOSED";
		})(exports$1.StompSocketState || (exports$1.StompSocketState = {}));
		/**
		* Possible activation state
		*/
		exports$1.ActivationState = void 0;
		(function(ActivationState) {
			ActivationState[ActivationState["ACTIVE"] = 0] = "ACTIVE";
			ActivationState[ActivationState["DEACTIVATING"] = 1] = "DEACTIVATING";
			ActivationState[ActivationState["INACTIVE"] = 2] = "INACTIVE";
		})(exports$1.ActivationState || (exports$1.ActivationState = {}));
		/**
		* Possible reconnection wait time modes
		*/
		exports$1.ReconnectionTimeMode = void 0;
		(function(ReconnectionTimeMode) {
			ReconnectionTimeMode[ReconnectionTimeMode["LINEAR"] = 0] = "LINEAR";
			ReconnectionTimeMode[ReconnectionTimeMode["EXPONENTIAL"] = 1] = "EXPONENTIAL";
		})(exports$1.ReconnectionTimeMode || (exports$1.ReconnectionTimeMode = {}));
		/**
		* Possible ticker strategies for outgoing heartbeat ping
		*/
		exports$1.TickerStrategy = void 0;
		(function(TickerStrategy) {
			TickerStrategy["Interval"] = "interval";
			TickerStrategy["Worker"] = "worker";
		})(exports$1.TickerStrategy || (exports$1.TickerStrategy = {}));
		class Ticker {
			constructor(_interval, _strategy = exports$1.TickerStrategy.Interval, _debug) {
				this._interval = _interval;
				this._strategy = _strategy;
				this._debug = _debug;
				this._workerScript = `
    var startTime = Date.now();
    setInterval(function() {
        self.postMessage(Date.now() - startTime);
    }, ${this._interval});
  `;
			}
			start(tick) {
				this.stop();
				if (this.shouldUseWorker()) this.runWorker(tick);
				else this.runInterval(tick);
			}
			stop() {
				this.disposeWorker();
				this.disposeInterval();
			}
			shouldUseWorker() {
				return typeof Worker !== "undefined" && this._strategy === exports$1.TickerStrategy.Worker;
			}
			runWorker(tick) {
				this._debug("Using runWorker for outgoing pings");
				if (!this._worker) {
					this._worker = new Worker(URL.createObjectURL(new Blob([this._workerScript], { type: "text/javascript" })));
					this._worker.onmessage = (message) => tick(message.data);
				}
			}
			runInterval(tick) {
				this._debug("Using runInterval for outgoing pings");
				if (!this._timer) {
					const startTime = Date.now();
					this._timer = setInterval(() => {
						tick(Date.now() - startTime);
					}, this._interval);
				}
			}
			disposeWorker() {
				if (this._worker) {
					this._worker.terminate();
					delete this._worker;
					this._debug("Outgoing ping disposeWorker");
				}
			}
			disposeInterval() {
				if (this._timer) {
					clearInterval(this._timer);
					delete this._timer;
					this._debug("Outgoing ping disposeInterval");
				}
			}
		}
		/**
		* Supported STOMP versions
		*
		* Part of `@stomp/stompjs`.
		*/
		class Versions {
			/**
			* Takes an array of versions, typical elements '1.2', '1.1', or '1.0'
			*
			* You will be creating an instance of this class if you want to override
			* supported versions to be declared during STOMP handshake.
			*/
			constructor(versions) {
				this.versions = versions;
			}
			/**
			* Used as part of CONNECT STOMP Frame
			*/
			supportedVersions() {
				return this.versions.join(",");
			}
			/**
			* Used while creating a WebSocket
			*/
			protocolVersions() {
				return this.versions.map((x) => `v${x.replace(".", "")}.stomp`);
			}
		}
		/**
		* Indicates protocol version 1.0
		*/
		Versions.V1_0 = "1.0";
		/**
		* Indicates protocol version 1.1
		*/
		Versions.V1_1 = "1.1";
		/**
		* Indicates protocol version 1.2
		*/
		Versions.V1_2 = "1.2";
		/**
		* @internal
		*/
		Versions.default = new Versions([
			Versions.V1_2,
			Versions.V1_1,
			Versions.V1_0
		]);
		/**
		* The STOMP protocol handler
		*
		* Part of `@stomp/stompjs`.
		*
		* @internal
		*/
		class StompHandler {
			get connectedVersion() {
				return this._connectedVersion;
			}
			get connected() {
				return this._connected;
			}
			constructor(_client, _webSocket, config) {
				this._client = _client;
				this._webSocket = _webSocket;
				this._connected = false;
				this._serverFrameHandlers = {
					CONNECTED: (frame) => {
						this.debug(`connected to server ${frame.headers.server}`);
						this._connected = true;
						this._connectedVersion = frame.headers.version;
						if (this._connectedVersion === Versions.V1_2) this._escapeHeaderValues = true;
						this._setupHeartbeat(frame.headers);
						this.onConnect(frame);
					},
					MESSAGE: (frame) => {
						const subscription = frame.headers.subscription;
						const onReceive = this._subscriptions[subscription] || this.onUnhandledMessage;
						const message = frame;
						const client = this;
						const messageId = this._connectedVersion === Versions.V1_2 ? message.headers.ack : message.headers["message-id"];
						message.ack = (headers = {}) => {
							return client.ack(messageId, subscription, headers);
						};
						message.nack = (headers = {}) => {
							return client.nack(messageId, subscription, headers);
						};
						onReceive(message);
					},
					RECEIPT: (frame) => {
						const callback = this._receiptWatchers[frame.headers["receipt-id"]];
						if (callback) {
							callback(frame);
							delete this._receiptWatchers[frame.headers["receipt-id"]];
						} else this.onUnhandledReceipt(frame);
					},
					ERROR: (frame) => {
						this.onStompError(frame);
					}
				};
				this._counter = 0;
				this._subscriptions = {};
				this._receiptWatchers = {};
				this._partialData = "";
				this._escapeHeaderValues = false;
				this._lastServerActivityTS = Date.now();
				this.debug = config.debug;
				this.stompVersions = config.stompVersions;
				this.connectHeaders = config.connectHeaders;
				this.disconnectHeaders = config.disconnectHeaders;
				this.heartbeatIncoming = config.heartbeatIncoming;
				this.heartbeatToleranceMultiplier = config.heartbeatGracePeriods;
				this.heartbeatOutgoing = config.heartbeatOutgoing;
				this.splitLargeFrames = config.splitLargeFrames;
				this.maxWebSocketChunkSize = config.maxWebSocketChunkSize;
				this.forceBinaryWSFrames = config.forceBinaryWSFrames;
				this.logRawCommunication = config.logRawCommunication;
				this.appendMissingNULLonIncoming = config.appendMissingNULLonIncoming;
				this.discardWebsocketOnCommFailure = config.discardWebsocketOnCommFailure;
				this.onConnect = config.onConnect;
				this.onDisconnect = config.onDisconnect;
				this.onStompError = config.onStompError;
				this.onWebSocketClose = config.onWebSocketClose;
				this.onWebSocketError = config.onWebSocketError;
				this.onUnhandledMessage = config.onUnhandledMessage;
				this.onUnhandledReceipt = config.onUnhandledReceipt;
				this.onUnhandledFrame = config.onUnhandledFrame;
				this.onHeartbeatReceived = config.onHeartbeatReceived;
				this.onHeartbeatLost = config.onHeartbeatLost;
			}
			start() {
				const parser = new Parser((rawFrame) => {
					const frame = FrameImpl.fromRawFrame(rawFrame, this._escapeHeaderValues);
					if (!this.logRawCommunication) this.debug(`<<< ${frame}`);
					(this._serverFrameHandlers[frame.command] || this.onUnhandledFrame)(frame);
				}, () => {
					this.debug("<<< PONG");
					this.onHeartbeatReceived();
				});
				this._webSocket.onmessage = (evt) => {
					this.debug("Received data");
					this._lastServerActivityTS = Date.now();
					if (this.logRawCommunication) {
						const rawChunkAsString = evt.data instanceof ArrayBuffer ? new TextDecoder().decode(evt.data) : evt.data;
						this.debug(`<<< ${rawChunkAsString}`);
					}
					parser.parseChunk(evt.data, this.appendMissingNULLonIncoming);
				};
				this._webSocket.onclose = (closeEvent) => {
					this.debug(`Connection closed to ${this._webSocket.url}`);
					this._cleanUp();
					this.onWebSocketClose(closeEvent);
				};
				this._webSocket.onerror = (errorEvent) => {
					this.onWebSocketError(errorEvent);
				};
				const onOpen = () => {
					const connectHeaders = Object.assign({}, this.connectHeaders);
					this.debug("Web Socket Opened...");
					connectHeaders["accept-version"] = this.stompVersions.supportedVersions();
					connectHeaders["heart-beat"] = [this.heartbeatOutgoing, this.heartbeatIncoming].join(",");
					this._transmit({
						command: "CONNECT",
						headers: connectHeaders
					});
				};
				if (this._webSocket.readyState === exports$1.StompSocketState.OPEN) onOpen();
				else this._webSocket.onopen = onOpen;
			}
			_setupHeartbeat(headers) {
				if (headers.version !== Versions.V1_1 && headers.version !== Versions.V1_2) return;
				if (!headers["heart-beat"]) return;
				const [serverOutgoing, serverIncoming] = headers["heart-beat"].split(",").map((v) => parseInt(v, 10));
				if (this.heartbeatOutgoing !== 0 && serverIncoming !== 0) {
					const ttl = Math.max(this.heartbeatOutgoing, serverIncoming);
					this.debug(`send PING every ${ttl}ms`);
					this._pinger = new Ticker(ttl, this._client.heartbeatStrategy, this.debug);
					this._pinger.start(() => {
						if (this._webSocket.readyState === exports$1.StompSocketState.OPEN) {
							this._webSocket.send(BYTE.LF);
							this.debug(">>> PING");
						}
					});
				}
				if (this.heartbeatIncoming !== 0 && serverOutgoing !== 0) {
					const ttl = Math.max(this.heartbeatIncoming, serverOutgoing);
					this.debug(`check PONG every ${ttl}ms`);
					this._ponger = setInterval(() => {
						const delta = Date.now() - this._lastServerActivityTS;
						if (delta > ttl * this.heartbeatToleranceMultiplier) {
							this.debug(`did not receive server activity for the last ${delta}ms`);
							this.onHeartbeatLost();
							this._closeOrDiscardWebsocket();
						}
					}, ttl);
				}
			}
			_closeOrDiscardWebsocket() {
				if (this.discardWebsocketOnCommFailure) {
					this.debug("Discarding websocket, the underlying socket may linger for a while");
					this.discardWebsocket();
				} else {
					this.debug("Issuing close on the websocket");
					this._closeWebsocket();
				}
			}
			forceDisconnect() {
				if (this._webSocket) {
					if (this._webSocket.readyState === exports$1.StompSocketState.CONNECTING || this._webSocket.readyState === exports$1.StompSocketState.OPEN) this._closeOrDiscardWebsocket();
				}
			}
			_closeWebsocket() {
				this._webSocket.onmessage = () => {};
				this._webSocket.close();
			}
			discardWebsocket() {
				if (typeof this._webSocket.terminate !== "function") augmentWebsocket(this._webSocket, (msg) => this.debug(msg));
				this._webSocket.terminate();
			}
			_transmit(params) {
				const { command, headers, body, binaryBody, skipContentLengthHeader } = params;
				const frame = new FrameImpl({
					command,
					headers,
					body,
					binaryBody,
					escapeHeaderValues: this._escapeHeaderValues,
					skipContentLengthHeader
				});
				let rawChunk = frame.serialize();
				if (this.logRawCommunication) this.debug(`>>> ${rawChunk}`);
				else this.debug(`>>> ${frame}`);
				if (this.forceBinaryWSFrames && typeof rawChunk === "string") rawChunk = new TextEncoder().encode(rawChunk);
				if (typeof rawChunk !== "string" || !this.splitLargeFrames) this._webSocket.send(rawChunk);
				else {
					let out = rawChunk;
					while (out.length > 0) {
						const chunk = out.substring(0, this.maxWebSocketChunkSize);
						out = out.substring(this.maxWebSocketChunkSize);
						this._webSocket.send(chunk);
						this.debug(`chunk sent = ${chunk.length}, remaining = ${out.length}`);
					}
				}
			}
			dispose() {
				if (this.connected) try {
					const disconnectHeaders = Object.assign({}, this.disconnectHeaders);
					if (!disconnectHeaders.receipt) disconnectHeaders.receipt = `close-${this._counter++}`;
					this.watchForReceipt(disconnectHeaders.receipt, (frame) => {
						this._closeWebsocket();
						this._cleanUp();
						this.onDisconnect(frame);
					});
					this._transmit({
						command: "DISCONNECT",
						headers: disconnectHeaders
					});
				} catch (error) {
					this.debug(`Ignoring error during disconnect ${error}`);
				}
				else if (this._webSocket.readyState === exports$1.StompSocketState.CONNECTING || this._webSocket.readyState === exports$1.StompSocketState.OPEN) this._closeWebsocket();
			}
			_cleanUp() {
				this._connected = false;
				if (this._pinger) {
					this._pinger.stop();
					this._pinger = void 0;
				}
				if (this._ponger) {
					clearInterval(this._ponger);
					this._ponger = void 0;
				}
			}
			publish(params) {
				const { destination, headers, body, binaryBody, skipContentLengthHeader } = params;
				const hdrs = Object.assign({ destination }, headers);
				this._transmit({
					command: "SEND",
					headers: hdrs,
					body,
					binaryBody,
					skipContentLengthHeader
				});
			}
			watchForReceipt(receiptId, callback) {
				this._receiptWatchers[receiptId] = callback;
			}
			subscribe(destination, callback, headers = {}) {
				headers = Object.assign({}, headers);
				if (!headers.id) headers.id = `sub-${this._counter++}`;
				headers.destination = destination;
				this._subscriptions[headers.id] = callback;
				this._transmit({
					command: "SUBSCRIBE",
					headers
				});
				const client = this;
				return {
					id: headers.id,
					unsubscribe(hdrs) {
						return client.unsubscribe(headers.id, hdrs);
					}
				};
			}
			unsubscribe(id, headers = {}) {
				headers = Object.assign({}, headers);
				delete this._subscriptions[id];
				headers.id = id;
				this._transmit({
					command: "UNSUBSCRIBE",
					headers
				});
			}
			begin(transactionId) {
				const txId = transactionId || `tx-${this._counter++}`;
				this._transmit({
					command: "BEGIN",
					headers: { transaction: txId }
				});
				const client = this;
				return {
					id: txId,
					commit() {
						client.commit(txId);
					},
					abort() {
						client.abort(txId);
					}
				};
			}
			commit(transactionId) {
				this._transmit({
					command: "COMMIT",
					headers: { transaction: transactionId }
				});
			}
			abort(transactionId) {
				this._transmit({
					command: "ABORT",
					headers: { transaction: transactionId }
				});
			}
			ack(messageId, subscriptionId, headers = {}) {
				headers = Object.assign({}, headers);
				if (this._connectedVersion === Versions.V1_2) headers.id = messageId;
				else headers["message-id"] = messageId;
				headers.subscription = subscriptionId;
				this._transmit({
					command: "ACK",
					headers
				});
			}
			nack(messageId, subscriptionId, headers = {}) {
				headers = Object.assign({}, headers);
				if (this._connectedVersion === Versions.V1_2) headers.id = messageId;
				else headers["message-id"] = messageId;
				headers.subscription = subscriptionId;
				return this._transmit({
					command: "NACK",
					headers
				});
			}
		}
		/**
		* STOMP Client Class.
		*
		* Part of `@stomp/stompjs`.
		*
		* This class provides a robust implementation for connecting to and interacting with a
		* STOMP-compliant messaging broker over WebSocket. It supports STOMP versions 1.2, 1.1, and 1.0.
		*
		* Features:
		* - Handles automatic reconnections.
		* - Supports heartbeat mechanisms to detect and report communication failures.
		* - Allows customization of connection and WebSocket behaviors through configurations.
		* - Compatible with both browser environments and Node.js with polyfill support for WebSocket.
		*/
		class Client {
			/**
			* Provides access to the underlying WebSocket instance.
			* This property is **read-only**.
			*
			* Example:
			* ```javascript
			* const webSocket = client.webSocket;
			* if (webSocket) {
			*   console.log('WebSocket is connected:', webSocket.readyState === WebSocket.OPEN);
			* }
			* ```
			*
			* **Caution:**
			* Directly interacting with the WebSocket instance (e.g., sending or receiving frames)
			* can interfere with the proper functioning of this library. Such actions may cause
			* unexpected behavior, disconnections, or invalid state in the library's internal mechanisms.
			*
			* Instead, use the library's provided methods to manage STOMP communication.
			*
			* @returns The WebSocket instance used by the STOMP handler, or `undefined` if not connected.
			*/
			get webSocket() {
				return this._stompHandler?._webSocket;
			}
			/**
			* Allows customization of the disconnection headers.
			*
			* Any changes made during an active session will also be applied immediately.
			*
			* Example:
			* ```javascript
			* client.disconnectHeaders = {
			*   receipt: 'custom-receipt-id'
			* };
			* ```
			*/
			get disconnectHeaders() {
				return this._disconnectHeaders;
			}
			set disconnectHeaders(value) {
				this._disconnectHeaders = value;
				if (this._stompHandler) this._stompHandler.disconnectHeaders = this._disconnectHeaders;
			}
			/**
			* Indicates whether there is an active connection to the STOMP broker.
			*
			* Usage:
			* ```javascript
			* if (client.connected) {
			*   console.log('Client is connected to the broker.');
			* } else {
			*   console.log('No connection to the broker.');
			* }
			* ```
			*
			* @returns `true` if the client is currently connected, `false` otherwise.
			*/
			get connected() {
				return !!this._stompHandler && this._stompHandler.connected;
			}
			/**
			* The version of the STOMP protocol negotiated with the server during connection.
			*
			* This is a **read-only** property and reflects the negotiated protocol version after
			* a successful connection.
			*
			* Example:
			* ```javascript
			* console.log('Connected STOMP version:', client.connectedVersion);
			* ```
			*
			* @returns The negotiated STOMP protocol version or `undefined` if not connected.
			*/
			get connectedVersion() {
				return this._stompHandler ? this._stompHandler.connectedVersion : void 0;
			}
			/**
			* Indicates whether the client is currently active.
			*
			* A client is considered active if it is connected or actively attempting to reconnect.
			*
			* Example:
			* ```javascript
			* if (client.active) {
			*   console.log('The client is active.');
			* } else {
			*   console.log('The client is inactive.');
			* }
			* ```
			*
			* @returns `true` if the client is active, otherwise `false`.
			*/
			get active() {
				return this.state === exports$1.ActivationState.ACTIVE;
			}
			_changeState(state) {
				this.state = state;
				this.onChangeState(state);
			}
			/**
			* Constructs a new STOMP client instance.
			*
			* The constructor initializes default values and sets up no-op callbacks for all events.
			* Configuration can be passed during construction, or updated later using `configure`.
			*
			* Example:
			* ```javascript
			* const client = new Client({
			*   brokerURL: 'wss://broker.example.com',
			*   reconnectDelay: 5000
			* });
			* ```
			*
			* @param conf Optional configuration object to initialize the client with.
			*/
			constructor(conf = {}) {
				/**
				* STOMP protocol versions to use during the handshake. By default, the client will attempt
				* versions `1.2`, `1.1`, and `1.0` in descending order of preference.
				*
				* Example:
				* ```javascript
				* // Configure the client to only use versions 1.1 and 1.0
				* client.stompVersions = new Versions(['1.1', '1.0']);
				* ```
				*/
				this.stompVersions = Versions.default;
				/**
				* Timeout for establishing STOMP connection, in milliseconds.
				*
				* If the connection is not established within this period, the attempt will fail.
				* The default is `0`, meaning no timeout is set for connection attempts.
				*
				* Example:
				* ```javascript
				* client.connectionTimeout = 5000; // Fail connection if not established in 5 seconds
				* ```
				*/
				this.connectionTimeout = 0;
				/**
				* Delay (in milliseconds) between reconnection attempts if the connection drops.
				*
				* Set to `0` to disable automatic reconnections. The default value is `5000` ms (5 seconds).
				*
				* Example:
				* ```javascript
				* client.reconnectDelay = 3000; // Attempt reconnection every 3 seconds
				* client.reconnectDelay = 0; // Disable automatic reconnection
				* ```
				*/
				this.reconnectDelay = 5e3;
				/**
				* The next reconnection delay, used internally.
				* Initialized to the value of [Client#reconnectDelay]{@link Client#reconnectDelay}, and it may
				* dynamically change based on [Client#reconnectTimeMode]{@link Client#reconnectTimeMode}.
				*/
				this._nextReconnectDelay = 0;
				/**
				* Maximum delay (in milliseconds) between reconnection attempts when using exponential backoff.
				*
				* Default is 15 minutes (`15 * 60 * 1000` milliseconds). If `0`, there will be no upper limit.
				*
				* Example:
				* ```javascript
				* client.maxReconnectDelay = 10000; // Maximum wait time is 10 seconds
				* ```
				*/
				this.maxReconnectDelay = 9e5;
				/**
				* Mode for determining the time interval between reconnection attempts.
				*
				* Available modes:
				* - `ReconnectionTimeMode.LINEAR` (default): Fixed delays between reconnection attempts.
				* - `ReconnectionTimeMode.EXPONENTIAL`: Delay doubles after each attempt, capped by [maxReconnectDelay]{@link Client#maxReconnectDelay}.
				*
				* Example:
				* ```javascript
				* client.reconnectTimeMode = ReconnectionTimeMode.EXPONENTIAL;
				* client.reconnectDelay = 200; // Initial delay of 200 ms, doubles with each attempt
				* client.maxReconnectDelay = 2 * 60 * 1000; // Cap delay at 10 minutes
				* ```
				*/
				this.reconnectTimeMode = exports$1.ReconnectionTimeMode.LINEAR;
				/**
				* Interval (in milliseconds) for receiving heartbeat signals from the server.
				*
				* Specifies the expected frequency of heartbeats sent by the server. Set to `0` to disable.
				*
				* Example:
				* ```javascript
				* client.heartbeatIncoming = 10000; // Expect a heartbeat every 10 seconds
				* ```
				*/
				this.heartbeatIncoming = 1e4;
				/**
				* Multiplier for adjusting tolerance when processing heartbeat signals.
				*
				* Tolerance level is calculated using the multiplier:
				* `tolerance = heartbeatIncoming * heartbeatToleranceMultiplier`.
				* This helps account for delays in network communication or variations in timings.
				*
				* Default value is `2`.
				*
				* Example:
				* ```javascript
				* client.heartbeatToleranceMultiplier = 2.5; // Tolerates longer delays
				* ```
				*/
				this.heartbeatToleranceMultiplier = 2;
				/**
				* Interval (in milliseconds) for sending heartbeat signals to the server.
				*
				* Specifies how frequently heartbeats should be sent to the server. Set to `0` to disable.
				*
				* Example:
				* ```javascript
				* client.heartbeatOutgoing = 5000; // Send a heartbeat every 5 seconds
				* ```
				*/
				this.heartbeatOutgoing = 1e4;
				/**
				* Strategy for sending outgoing heartbeats.
				*
				* Options:
				* - `TickerStrategy.Worker`: Uses Web Workers for sending heartbeats (recommended for long-running or background sessions).
				* - `TickerStrategy.Interval`: Uses standard JavaScript `setInterval` (default).
				*
				* Note:
				* - If Web Workers are unavailable (e.g., in Node.js), the `Interval` strategy is used automatically.
				* - Web Workers are preferable in browsers for reducing disconnects when tabs are in the background.
				*
				* Example:
				* ```javascript
				* client.heartbeatStrategy = TickerStrategy.Worker;
				* ```
				*/
				this.heartbeatStrategy = exports$1.TickerStrategy.Interval;
				/**
				* Enables splitting of large text WebSocket frames into smaller chunks.
				*
				* This setting is enabled for brokers that support only chunked messages (e.g., Java Spring-based brokers).
				* Default is `false`.
				*
				* Warning:
				* - Should not be used with WebSocket-compliant brokers, as chunking may cause large message failures.
				* - Binary WebSocket frames are never split.
				*
				* Example:
				* ```javascript
				* client.splitLargeFrames = true;
				* client.maxWebSocketChunkSize = 4096; // Allow chunks of 4 KB
				* ```
				*/
				this.splitLargeFrames = false;
				/**
				* Maximum size (in bytes) for individual WebSocket chunks if [splitLargeFrames]{@link Client#splitLargeFrames} is enabled.
				*
				* Default is 8 KB (`8 * 1024` bytes). This value has no effect if [splitLargeFrames]{@link Client#splitLargeFrames} is `false`.
				*/
				this.maxWebSocketChunkSize = 8192;
				/**
				* Forces all WebSocket frames to use binary transport, irrespective of payload type.
				*
				* Default behavior determines frame type based on payload (e.g., binary data for ArrayBuffers).
				*
				* Example:
				* ```javascript
				* client.forceBinaryWSFrames = true;
				* ```
				*/
				this.forceBinaryWSFrames = false;
				/**
				* Workaround for a React Native WebSocket bug, where messages containing `NULL` are chopped.
				*
				* Enabling this appends a `NULL` character to incoming frames to ensure they remain valid STOMP packets.
				*
				* Warning:
				* - For brokers that split large messages, this may cause data loss or connection termination.
				*
				* Example:
				* ```javascript
				* client.appendMissingNULLonIncoming = true;
				* ```
				*/
				this.appendMissingNULLonIncoming = false;
				/**
				* Instruct the library to immediately terminate the socket on communication failures, even
				* before the WebSocket is completely closed.
				*
				* This is particularly useful in browser environments where WebSocket closure may get delayed,
				* causing prolonged reconnection intervals under certain failure conditions.
				*
				*
				* Example:
				* ```javascript
				* client.discardWebsocketOnCommFailure = true; // Enable aggressive closing of WebSocket
				* ```
				*
				* Default value: `false`.
				*/
				this.discardWebsocketOnCommFailure = false;
				/**
				* Current activation state of the client.
				*
				* Possible states:
				* - `ActivationState.ACTIVE`: Client is connected or actively attempting to connect.
				* - `ActivationState.INACTIVE`: Client is disconnected and not attempting to reconnect.
				* - `ActivationState.DEACTIVATING`: Client is in the process of disconnecting.
				*
				* Note: The client may transition directly from `ACTIVE` to `INACTIVE` without entering
				* the `DEACTIVATING` state.
				*/
				this.state = exports$1.ActivationState.INACTIVE;
				const noOp = () => {};
				this.debug = noOp;
				this.beforeConnect = noOp;
				this.onConnect = noOp;
				this.onDisconnect = noOp;
				this.onUnhandledMessage = noOp;
				this.onUnhandledReceipt = noOp;
				this.onUnhandledFrame = noOp;
				this.onHeartbeatReceived = noOp;
				this.onHeartbeatLost = noOp;
				this.onStompError = noOp;
				this.onWebSocketClose = noOp;
				this.onWebSocketError = noOp;
				this.logRawCommunication = false;
				this.onChangeState = noOp;
				this.connectHeaders = {};
				this._disconnectHeaders = {};
				this.configure(conf);
			}
			/**
			* Updates the client's configuration.
			*
			* All properties in the provided configuration object will override the current settings.
			*
			* Additionally, a warning is logged if `maxReconnectDelay` is configured to a
			* value lower than `reconnectDelay`, and `maxReconnectDelay` is adjusted to match `reconnectDelay`.
			*
			* Example:
			* ```javascript
			* client.configure({
			*   reconnectDelay: 3000,
			*   maxReconnectDelay: 10000
			* });
			* ```
			*
			* @param conf Configuration object containing the new settings.
			*/
			configure(conf) {
				Object.assign(this, conf);
				if (this.maxReconnectDelay > 0 && this.maxReconnectDelay < this.reconnectDelay) {
					this.debug(`Warning: maxReconnectDelay (${this.maxReconnectDelay}ms) is less than reconnectDelay (${this.reconnectDelay}ms). Using reconnectDelay as the maxReconnectDelay delay.`);
					this.maxReconnectDelay = this.reconnectDelay;
				}
			}
			/**
			* Activates the client, initiating a connection to the STOMP broker.
			*
			* On activation, the client attempts to connect and sets its state to `ACTIVE`. If the connection
			* is lost, it will automatically retry based on `reconnectDelay` or `maxReconnectDelay`. If
			* `reconnectTimeMode` is set to `EXPONENTIAL`, the reconnect delay increases exponentially.
			*
			* To stop reconnection attempts and disconnect, call [Client#deactivate]{@link Client#deactivate}.
			*
			* Example:
			* ```javascript
			* client.activate(); // Connect to the broker
			* ```
			*
			* If the client is currently `DEACTIVATING`, connection is delayed until the deactivation process completes.
			*/
			activate() {
				const _activate = () => {
					if (this.active) {
						this.debug("Already ACTIVE, ignoring request to activate");
						return;
					}
					this._changeState(exports$1.ActivationState.ACTIVE);
					this._nextReconnectDelay = this.reconnectDelay;
					this._connect();
				};
				if (this.state === exports$1.ActivationState.DEACTIVATING) {
					this.debug("Waiting for deactivation to finish before activating");
					this.deactivate().then(() => {
						_activate();
					});
				} else _activate();
			}
			async _connect() {
				await this.beforeConnect(this);
				if (this._stompHandler) {
					this.debug("There is already a stompHandler, skipping the call to connect");
					return;
				}
				if (!this.active) {
					this.debug("Client has been marked inactive, will not attempt to connect");
					return;
				}
				if (this.connectionTimeout > 0) {
					if (this._connectionWatcher) clearTimeout(this._connectionWatcher);
					this._connectionWatcher = setTimeout(() => {
						if (this.connected) return;
						this.debug(`Connection not established in ${this.connectionTimeout}ms, closing socket`);
						this.forceDisconnect();
					}, this.connectionTimeout);
				}
				this.debug("Opening Web Socket...");
				const webSocket = this._createWebSocket();
				this._stompHandler = new StompHandler(this, webSocket, {
					debug: this.debug,
					stompVersions: this.stompVersions,
					connectHeaders: this.connectHeaders,
					disconnectHeaders: this._disconnectHeaders,
					heartbeatIncoming: this.heartbeatIncoming,
					heartbeatGracePeriods: this.heartbeatToleranceMultiplier,
					heartbeatOutgoing: this.heartbeatOutgoing,
					heartbeatStrategy: this.heartbeatStrategy,
					splitLargeFrames: this.splitLargeFrames,
					maxWebSocketChunkSize: this.maxWebSocketChunkSize,
					forceBinaryWSFrames: this.forceBinaryWSFrames,
					logRawCommunication: this.logRawCommunication,
					appendMissingNULLonIncoming: this.appendMissingNULLonIncoming,
					discardWebsocketOnCommFailure: this.discardWebsocketOnCommFailure,
					onConnect: (frame) => {
						if (this._connectionWatcher) {
							clearTimeout(this._connectionWatcher);
							this._connectionWatcher = void 0;
						}
						this._nextReconnectDelay = this.reconnectDelay;
						if (!this.active) {
							this.debug("STOMP got connected while deactivate was issued, will disconnect now");
							this._disposeStompHandler();
							return;
						}
						this.onConnect(frame);
					},
					onDisconnect: (frame) => {
						this.onDisconnect(frame);
					},
					onStompError: (frame) => {
						this.onStompError(frame);
					},
					onWebSocketClose: (evt) => {
						this._stompHandler = void 0;
						if (this.state === exports$1.ActivationState.DEACTIVATING) this._changeState(exports$1.ActivationState.INACTIVE);
						this.onWebSocketClose(evt);
						if (this.active) this._schedule_reconnect();
					},
					onWebSocketError: (evt) => {
						this.onWebSocketError(evt);
					},
					onUnhandledMessage: (message) => {
						this.onUnhandledMessage(message);
					},
					onUnhandledReceipt: (frame) => {
						this.onUnhandledReceipt(frame);
					},
					onUnhandledFrame: (frame) => {
						this.onUnhandledFrame(frame);
					},
					onHeartbeatReceived: () => {
						this.onHeartbeatReceived();
					},
					onHeartbeatLost: () => {
						this.onHeartbeatLost();
					}
				});
				this._stompHandler.start();
			}
			_createWebSocket() {
				let webSocket;
				if (this.webSocketFactory) webSocket = this.webSocketFactory();
				else if (this.brokerURL) webSocket = new WebSocket(this.brokerURL, this.stompVersions.protocolVersions());
				else throw new Error("Either brokerURL or webSocketFactory must be provided");
				webSocket.binaryType = "arraybuffer";
				return webSocket;
			}
			_schedule_reconnect() {
				if (this._nextReconnectDelay > 0) {
					this.debug(`STOMP: scheduling reconnection in ${this._nextReconnectDelay}ms`);
					this._reconnector = setTimeout(() => {
						if (this.reconnectTimeMode === exports$1.ReconnectionTimeMode.EXPONENTIAL) {
							this._nextReconnectDelay = this._nextReconnectDelay * 2;
							if (this.maxReconnectDelay !== 0) this._nextReconnectDelay = Math.min(this._nextReconnectDelay, this.maxReconnectDelay);
						}
						this._connect();
					}, this._nextReconnectDelay);
				}
			}
			/**
			* Disconnects the client and stops the automatic reconnection loop.
			*
			* If there is an active STOMP connection at the time of invocation, the appropriate callbacks
			* will be triggered during the shutdown sequence. Once deactivated, the client will enter the
			* `INACTIVE` state, and no further reconnection attempts will be made.
			*
			* **Behavior**:
			* - If there is no active WebSocket connection, this method resolves immediately.
			* - If there is an active connection, the method waits for the underlying WebSocket
			*   to properly close before resolving.
			* - Multiple calls to this method are safe. Each invocation resolves upon completion.
			* - To reactivate, call [Client#activate]{@link Client#activate}.
			*
			* **Experimental Option:**
			* - By specifying the `force: true` option, the WebSocket connection is discarded immediately,
			*   bypassing both the STOMP and WebSocket shutdown sequences.
			* - **Caution:** Using `force: true` may leave the WebSocket in an inconsistent state,
			*   and brokers may not immediately detect the termination.
			*
			* Example:
			* ```javascript
			* // Graceful disconnect
			* await client.deactivate();
			*
			* // Forced disconnect to speed up shutdown when the connection is stale
			* await client.deactivate({ force: true });
			* ```
			*
			* @param options Configuration options for deactivation. Use `force: true` for immediate shutdown.
			* @returns A Promise that resolves when the deactivation process completes.
			*/
			async deactivate(options = {}) {
				const force = options.force || false;
				const needToDispose = this.active;
				let retPromise;
				if (this.state === exports$1.ActivationState.INACTIVE) {
					this.debug(`Already INACTIVE, nothing more to do`);
					return Promise.resolve();
				}
				this._changeState(exports$1.ActivationState.DEACTIVATING);
				this._nextReconnectDelay = 0;
				if (this._reconnector) {
					clearTimeout(this._reconnector);
					this._reconnector = void 0;
				}
				if (this._stompHandler && this.webSocket.readyState !== exports$1.StompSocketState.CLOSED) {
					const origOnWebSocketClose = this._stompHandler.onWebSocketClose;
					retPromise = new Promise((resolve, reject) => {
						this._stompHandler.onWebSocketClose = (evt) => {
							origOnWebSocketClose(evt);
							resolve();
						};
					});
				} else {
					this._changeState(exports$1.ActivationState.INACTIVE);
					return Promise.resolve();
				}
				if (force) this._stompHandler?.discardWebsocket();
				else if (needToDispose) this._disposeStompHandler();
				return retPromise;
			}
			/**
			* Forces a disconnect by directly closing the WebSocket.
			*
			* Unlike a normal disconnect, this does not send a DISCONNECT sequence to the broker but
			* instead closes the WebSocket connection directly. After forcing a disconnect, the client
			* will automatically attempt to reconnect based on its `reconnectDelay` configuration.
			*
			* **Note:** To prevent further reconnect attempts, call [Client#deactivate]{@link Client#deactivate}.
			*
			* Example:
			* ```javascript
			* client.forceDisconnect();
			* ```
			*/
			forceDisconnect() {
				if (this._stompHandler) this._stompHandler.forceDisconnect();
			}
			_disposeStompHandler() {
				if (this._stompHandler) this._stompHandler.dispose();
			}
			/**
			* Sends a message to the specified destination on the STOMP broker.
			*
			* The `body` must be a `string`. For non-string payloads (e.g., JSON), encode it as a string before sending.
			* If sending binary data, use the `binaryBody` parameter as a [Uint8Array](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Uint8Array).
			*
			* **Content-Length Behavior**:
			* - For non-binary messages, the `content-length` header is added by default.
			* - The `content-length` header can be skipped for text frames by setting `skipContentLengthHeader: true` in the parameters.
			* - For binary messages, the `content-length` header is always included.
			*
			* **Notes**:
			* - Ensure that brokers support binary frames before using `binaryBody`.
			* - Sending messages with NULL octets and missing `content-length` headers can cause brokers to disconnect and throw errors.
			*
			* Example:
			* ```javascript
			* // Basic text message
			* client.publish({ destination: "/queue/test", body: "Hello, STOMP" });
			*
			* // Text message with additional headers
			* client.publish({ destination: "/queue/test", headers: { priority: 9 }, body: "Hello, STOMP" });
			*
			* // Skip content-length header
			* client.publish({ destination: "/queue/test", body: "Hello, STOMP", skipContentLengthHeader: true });
			*
			* // Binary message
			* const binaryData = new Uint8Array([1, 2, 3, 4]);
			* client.publish({
			*   destination: '/topic/special',
			*   binaryBody: binaryData,
			*   headers: { 'content-type': 'application/octet-stream' }
			* });
			* ```
			*/
			publish(params) {
				this._checkConnection();
				this._stompHandler.publish(params);
			}
			_checkConnection() {
				if (!this.connected) throw new TypeError("There is no underlying STOMP connection");
			}
			/**
			* Monitors for a receipt acknowledgment from the broker for specific operations.
			*
			* Add a `receipt` header to the operation (like subscribe or publish), and use this method with
			* the same receipt ID to detect when the broker has acknowledged the operation's completion.
			*
			* The callback is invoked with the corresponding {@link IFrame} when the receipt is received.
			*
			* Example:
			* ```javascript
			* const receiptId = "unique-receipt-id";
			*
			* client.watchForReceipt(receiptId, (frame) => {
			*   console.log("Operation acknowledged by the broker:", frame);
			* });
			*
			* // Attach the receipt header to an operation
			* client.publish({ destination: "/queue/test", headers: { receipt: receiptId }, body: "Hello" });
			* ```
			*
			* @param receiptId Unique identifier for the receipt.
			* @param callback Callback function invoked on receiving the RECEIPT frame.
			*/
			watchForReceipt(receiptId, callback) {
				this._checkConnection();
				this._stompHandler.watchForReceipt(receiptId, callback);
			}
			/**
			* Subscribes to a destination on the STOMP broker.
			*
			* The callback is triggered for each message received from the subscribed destination. The message
			* is passed as an {@link IMessage} instance.
			*
			* **Subscription ID**:
			* - If no `id` is provided in `headers`, the library generates a unique subscription ID automatically.
			* - Provide an explicit `id` in `headers` if you wish to manage the subscription ID manually.
			*
			* Example:
			* ```javascript
			* const callback = (message) => {
			*   console.log("Received message:", message.body);
			* };
			*
			* // Auto-generated subscription ID
			* const subscription = client.subscribe("/queue/test", callback);
			*
			* // Explicit subscription ID
			* const mySubId = "my-subscription-id";
			* const subscription = client.subscribe("/queue/test", callback, { id: mySubId });
			* ```
			*
			* @param destination Destination to subscribe to.
			* @param callback Function invoked for each received message.
			* @param headers Optional headers for subscription, such as `id`.
			* @returns A {@link StompSubscription} which can be used to manage the subscription.
			*/
			subscribe(destination, callback, headers = {}) {
				this._checkConnection();
				return this._stompHandler.subscribe(destination, callback, headers);
			}
			/**
			* Unsubscribes from a subscription on the STOMP broker.
			*
			* Prefer using the `unsubscribe` method directly on the {@link StompSubscription} returned from `subscribe` for cleaner management:
			* ```javascript
			* const subscription = client.subscribe("/queue/test", callback);
			* // Unsubscribe using the subscription object
			* subscription.unsubscribe();
			* ```
			*
			* This method can also be used directly with the subscription ID.
			*
			* Example:
			* ```javascript
			* client.unsubscribe("my-subscription-id");
			* ```
			*
			* @param id Subscription ID to unsubscribe.
			* @param headers Optional headers to pass for the UNSUBSCRIBE frame.
			*/
			unsubscribe(id, headers = {}) {
				this._checkConnection();
				this._stompHandler.unsubscribe(id, headers);
			}
			/**
			* Starts a new transaction. The returned {@link ITransaction} object provides
			* methods for [commit]{@link ITransaction#commit} and [abort]{@link ITransaction#abort}.
			*
			* If `transactionId` is not provided, the library generates a unique ID internally.
			*
			* Example:
			* ```javascript
			* const tx = client.begin(); // Auto-generated ID
			*
			* // Or explicitly specify a transaction ID
			* const tx = client.begin("my-transaction-id");
			* ```
			*
			* @param transactionId Optional transaction ID.
			* @returns An instance of {@link ITransaction}.
			*/
			begin(transactionId) {
				this._checkConnection();
				return this._stompHandler.begin(transactionId);
			}
			/**
			* Commits a transaction.
			*
			* It is strongly recommended to call [commit]{@link ITransaction#commit} on
			* the transaction object returned by [client#begin]{@link Client#begin}.
			*
			* Example:
			* ```javascript
			* const tx = client.begin();
			* // Perform operations under this transaction
			* tx.commit();
			* ```
			*
			* @param transactionId The ID of the transaction to commit.
			*/
			commit(transactionId) {
				this._checkConnection();
				this._stompHandler.commit(transactionId);
			}
			/**
			* Aborts a transaction.
			*
			* It is strongly recommended to call [abort]{@link ITransaction#abort} directly
			* on the transaction object returned by [client#begin]{@link Client#begin}.
			*
			* Example:
			* ```javascript
			* const tx = client.begin();
			* // Perform operations under this transaction
			* tx.abort(); // Abort the transaction
			* ```
			*
			* @param transactionId The ID of the transaction to abort.
			*/
			abort(transactionId) {
				this._checkConnection();
				this._stompHandler.abort(transactionId);
			}
			/**
			* Acknowledges receipt of a message. Typically, this should be done by calling
			* [ack]{@link IMessage#ack} directly on the {@link IMessage} instance passed
			* to the subscription callback.
			*
			* Example:
			* ```javascript
			* const callback = (message) => {
			*   // Process the message
			*   message.ack(); // Acknowledge the message
			* };
			*
			* client.subscribe("/queue/example", callback, { ack: "client" });
			* ```
			*
			* @param messageId The ID of the message to acknowledge.
			* @param subscriptionId The ID of the subscription.
			* @param headers Optional headers for the acknowledgment frame.
			*/
			ack(messageId, subscriptionId, headers = {}) {
				this._checkConnection();
				this._stompHandler.ack(messageId, subscriptionId, headers);
			}
			/**
			* Rejects a message (negative acknowledgment). Like acknowledgments, this should
			* typically be done by calling [nack]{@link IMessage#nack} directly on the {@link IMessage}
			* instance passed to the subscription callback.
			*
			* Example:
			* ```javascript
			* const callback = (message) => {
			*   // Process the message
			*   if (isError(message)) {
			*     message.nack(); // Reject the message
			*   }
			* };
			*
			* client.subscribe("/queue/example", callback, { ack: "client" });
			* ```
			*
			* @param messageId The ID of the message to negatively acknowledge.
			* @param subscriptionId The ID of the subscription.
			* @param headers Optional headers for the NACK frame.
			*/
			nack(messageId, subscriptionId, headers = {}) {
				this._checkConnection();
				this._stompHandler.nack(messageId, subscriptionId, headers);
			}
		}
		/**
		* Configuration options for STOMP Client, each key corresponds to
		* field by the same name in {@link Client}. This can be passed to
		* the constructor of {@link Client} or to [Client#configure]{@link Client#configure}.
		*
		* Part of `@stomp/stompjs`.
		*/
		class StompConfig {}
		/**
		* STOMP headers. Many function calls will accept headers as parameters.
		* The headers sent by Broker will be available as [IFrame#headers]{@link IFrame#headers}.
		*
		* `key` and `value` must be valid strings.
		* In addition, `key` must not contain `CR`, `LF`, or `:`.
		*
		* Part of `@stomp/stompjs`.
		*/
		class StompHeaders {}
		/**
		* Part of `@stomp/stompjs`.
		*
		* @internal
		*/
		class HeartbeatInfo {
			constructor(client) {
				this.client = client;
			}
			get outgoing() {
				return this.client.heartbeatOutgoing;
			}
			set outgoing(value) {
				this.client.heartbeatOutgoing = value;
			}
			get incoming() {
				return this.client.heartbeatIncoming;
			}
			set incoming(value) {
				this.client.heartbeatIncoming = value;
			}
		}
		/**
		* Available for backward compatibility, please shift to using {@link Client}.
		*
		* **Deprecated**
		*
		* Part of `@stomp/stompjs`.
		*
		* To upgrade, please follow the [Upgrade Guide](https://stomp-js.github.io/guide/stompjs/upgrading-stompjs.html)
		*/
		class CompatClient extends Client {
			/**
			* Available for backward compatibility, please shift to using {@link Client}
			* and [Client#webSocketFactory]{@link Client#webSocketFactory}.
			*
			* **Deprecated**
			*
			* @internal
			*/
			constructor(webSocketFactory) {
				super();
				/**
				* It is no op now. No longer needed. Large packets work out of the box.
				*/
				this.maxWebSocketFrameSize = 16384;
				this._heartbeatInfo = new HeartbeatInfo(this);
				this.reconnect_delay = 0;
				this.webSocketFactory = webSocketFactory;
				this.debug = (...message) => {
					console.log(...message);
				};
			}
			_parseConnect(...args) {
				let closeEventCallback;
				let connectCallback;
				let errorCallback;
				let headers = {};
				if (args.length < 2) throw new Error("Connect requires at least 2 arguments");
				if (typeof args[1] === "function") [headers, connectCallback, errorCallback, closeEventCallback] = args;
				else switch (args.length) {
					case 6:
						[headers.login, headers.passcode, connectCallback, errorCallback, closeEventCallback, headers.host] = args;
						break;
					default: [headers.login, headers.passcode, connectCallback, errorCallback, closeEventCallback] = args;
				}
				return [
					headers,
					connectCallback,
					errorCallback,
					closeEventCallback
				];
			}
			/**
			* Available for backward compatibility, please shift to using [Client#activate]{@link Client#activate}.
			*
			* **Deprecated**
			*
			* The `connect` method accepts different number of arguments and types. See the Overloads list. Use the
			* version with headers to pass your broker specific options.
			*
			* overloads:
			* - connect(headers, connectCallback)
			* - connect(headers, connectCallback, errorCallback)
			* - connect(login, passcode, connectCallback)
			* - connect(login, passcode, connectCallback, errorCallback)
			* - connect(login, passcode, connectCallback, errorCallback, closeEventCallback)
			* - connect(login, passcode, connectCallback, errorCallback, closeEventCallback, host)
			*
			* params:
			* - headers, see [Client#connectHeaders]{@link Client#connectHeaders}
			* - connectCallback, see [Client#onConnect]{@link Client#onConnect}
			* - errorCallback, see [Client#onStompError]{@link Client#onStompError}
			* - closeEventCallback, see [Client#onWebSocketClose]{@link Client#onWebSocketClose}
			* - login [String], see [Client#connectHeaders](../classes/Client.html#connectHeaders)
			* - passcode [String], [Client#connectHeaders](../classes/Client.html#connectHeaders)
			* - host [String], see [Client#connectHeaders](../classes/Client.html#connectHeaders)
			*
			* To upgrade, please follow the [Upgrade Guide](../additional-documentation/upgrading.html)
			*/
			connect(...args) {
				const out = this._parseConnect(...args);
				if (out[0]) this.connectHeaders = out[0];
				if (out[1]) this.onConnect = out[1];
				if (out[2]) this.onStompError = out[2];
				if (out[3]) this.onWebSocketClose = out[3];
				super.activate();
			}
			/**
			* Available for backward compatibility, please shift to using [Client#deactivate]{@link Client#deactivate}.
			*
			* **Deprecated**
			*
			* See:
			* [Client#onDisconnect]{@link Client#onDisconnect}, and
			* [Client#disconnectHeaders]{@link Client#disconnectHeaders}
			*
			* To upgrade, please follow the [Upgrade Guide](../additional-documentation/upgrading.html)
			*/
			disconnect(disconnectCallback, headers = {}) {
				if (disconnectCallback) this.onDisconnect = disconnectCallback;
				this.disconnectHeaders = headers;
				super.deactivate();
			}
			/**
			* Available for backward compatibility, use [Client#publish]{@link Client#publish}.
			*
			* Send a message to a named destination. Refer to your STOMP broker documentation for types
			* and naming of destinations. The headers will, typically, be available to the subscriber.
			* However, there may be special purpose headers corresponding to your STOMP broker.
			*
			*  **Deprecated**, use [Client#publish]{@link Client#publish}
			*
			* Note: Body must be String. You will need to covert the payload to string in case it is not string (e.g. JSON)
			*
			* ```javascript
			*        client.send("/queue/test", {priority: 9}, "Hello, STOMP");
			*
			*        // If you want to send a message with a body, you must also pass the headers argument.
			*        client.send("/queue/test", {}, "Hello, STOMP");
			* ```
			*
			* To upgrade, please follow the [Upgrade Guide](../additional-documentation/upgrading.html)
			*/
			send(destination, headers = {}, body = "") {
				headers = Object.assign({}, headers);
				const skipContentLengthHeader = headers["content-length"] === false;
				if (skipContentLengthHeader) delete headers["content-length"];
				this.publish({
					destination,
					headers,
					body,
					skipContentLengthHeader
				});
			}
			/**
			* Available for backward compatibility, renamed to [Client#reconnectDelay]{@link Client#reconnectDelay}.
			*
			* **Deprecated**
			*/
			set reconnect_delay(value) {
				this.reconnectDelay = value;
			}
			/**
			* Available for backward compatibility, renamed to [Client#webSocket]{@link Client#webSocket}.
			*
			* **Deprecated**
			*/
			get ws() {
				return this.webSocket;
			}
			/**
			* Available for backward compatibility, renamed to [Client#connectedVersion]{@link Client#connectedVersion}.
			*
			* **Deprecated**
			*/
			get version() {
				return this.connectedVersion;
			}
			/**
			* Available for backward compatibility, renamed to [Client#onUnhandledMessage]{@link Client#onUnhandledMessage}.
			*
			* **Deprecated**
			*/
			get onreceive() {
				return this.onUnhandledMessage;
			}
			/**
			* Available for backward compatibility, renamed to [Client#onUnhandledMessage]{@link Client#onUnhandledMessage}.
			*
			* **Deprecated**
			*/
			set onreceive(value) {
				this.onUnhandledMessage = value;
			}
			/**
			* Available for backward compatibility, renamed to [Client#onUnhandledReceipt]{@link Client#onUnhandledReceipt}.
			* Prefer using [Client#watchForReceipt]{@link Client#watchForReceipt}.
			*
			* **Deprecated**
			*/
			get onreceipt() {
				return this.onUnhandledReceipt;
			}
			/**
			* Available for backward compatibility, renamed to [Client#onUnhandledReceipt]{@link Client#onUnhandledReceipt}.
			*
			* **Deprecated**
			*/
			set onreceipt(value) {
				this.onUnhandledReceipt = value;
			}
			/**
			* Available for backward compatibility, renamed to [Client#heartbeatIncoming]{@link Client#heartbeatIncoming}
			* [Client#heartbeatOutgoing]{@link Client#heartbeatOutgoing}.
			*
			* **Deprecated**
			*/
			get heartbeat() {
				return this._heartbeatInfo;
			}
			/**
			* Available for backward compatibility, renamed to [Client#heartbeatIncoming]{@link Client#heartbeatIncoming}
			* [Client#heartbeatOutgoing]{@link Client#heartbeatOutgoing}.
			*
			* **Deprecated**
			*/
			set heartbeat(value) {
				this.heartbeatIncoming = value.incoming;
				this.heartbeatOutgoing = value.outgoing;
			}
		}
		/**
		* STOMP Class, acts like a factory to create {@link Client}.
		*
		* Part of `@stomp/stompjs`.
		*
		* **Deprecated**
		*
		* It will be removed in next major version. Please switch to {@link Client}.
		*/
		class Stomp {
			/**
			* This method creates a WebSocket client that is connected to
			* the STOMP server located at the url.
			*
			* ```javascript
			*        var url = "ws://localhost:61614/stomp";
			*        var client = Stomp.client(url);
			* ```
			*
			* **Deprecated**
			*
			* It will be removed in next major version. Please switch to {@link Client}
			* using [Client#brokerURL]{@link Client#brokerURL}.
			*/
			static client(url, protocols) {
				if (protocols == null) protocols = Versions.default.protocolVersions();
				const wsFn = () => {
					return new (Stomp.WebSocketClass || WebSocket)(url, protocols);
				};
				return new CompatClient(wsFn);
			}
			/**
			* This method is an alternative to [Stomp#client]{@link Stomp#client} to let the user
			* specify the WebSocket to use (either a standard HTML5 WebSocket or
			* a similar object).
			*
			* In order to support reconnection, the function Client._connect should be callable more than once.
			* While reconnecting
			* a new instance of underlying transport (TCP Socket, WebSocket or SockJS) will be needed. So, this function
			* alternatively allows passing a function that should return a new instance of the underlying socket.
			*
			* ```javascript
			*        var client = Stomp.over(function(){
			*          return new WebSocket('ws://localhost:15674/ws')
			*        });
			* ```
			*
			* **Deprecated**
			*
			* It will be removed in next major version. Please switch to {@link Client}
			* using [Client#webSocketFactory]{@link Client#webSocketFactory}.
			*/
			static over(ws) {
				let wsFn;
				if (typeof ws === "function") wsFn = ws;
				else {
					console.warn("Stomp.over did not receive a factory, auto reconnect will not work. Please see https://stomp-js.github.io/api-docs/latest/classes/Stomp.html#over");
					wsFn = () => ws;
				}
				return new CompatClient(wsFn);
			}
		}
		/**
		* In case you need to use a non standard class for WebSocket.
		*
		* For example when using within NodeJS environment:
		*
		* ```javascript
		*        StompJs = require('../../esm5/');
		*        Stomp = StompJs.Stomp;
		*        Stomp.WebSocketClass = require('websocket').w3cwebsocket;
		* ```
		*
		* **Deprecated**
		*
		*
		* It will be removed in next major version. Please switch to {@link Client}
		* using [Client#webSocketFactory]{@link Client#webSocketFactory}.
		*/
		Stomp.WebSocketClass = null;
		exports$1.Client = Client;
		exports$1.CompatClient = CompatClient;
		exports$1.FrameImpl = FrameImpl;
		exports$1.Parser = Parser;
		exports$1.Stomp = Stomp;
		exports$1.StompConfig = StompConfig;
		exports$1.StompHeaders = StompHeaders;
		exports$1.Versions = Versions;
	}));
})))();
/**
* Connection lifecycle states for RxStomp.
*
* These states reflect the status of the underlying STOMP-over-WebSocket connection
* as well as the client's activation/deactivation lifecycle.
*
* Common usage:
* ```typescript
* rxStomp.connectionState$.subscribe((state) => {
*   switch (state) {
*     case RxStompState.CONNECTING:
*       console.log('Attempting to connect...');
*       break;
*     case RxStompState.OPEN:
*       console.log('Connected');
*       break;
*     case RxStompState.CLOSING:
*       console.log('Disconnecting...');
*       break;
*     case RxStompState.CLOSED:
*       console.log('Disconnected');
*       break;
*   }
* });
* ```
*
* Part of `@stomp/rx-stomp`
*/
var RxStompState;
(function(RxStompState) {
	/**
	* A connection attempt is in progress. This includes the WebSocket opening
	* and the STOMP CONNECT handshake phase.
	*/
	RxStompState[RxStompState["CONNECTING"] = 0] = "CONNECTING";
	/**
	* The STOMP session is fully established and operational.
	* Messages can be published, and subscriptions will receive messages.
	*/
	RxStompState[RxStompState["OPEN"] = 1] = "OPEN";
	/**
	* A graceful shutdown has been requested and is underway.
	* New subscriptions/publishes should be avoided during this phase.
	*/
	RxStompState[RxStompState["CLOSING"] = 2] = "CLOSING";
	/**
	* There is no active connection to the broker.
	* The client may be inactive or waiting to retry based on reconnection settings.
	*/
	RxStompState[RxStompState["CLOSED"] = 3] = "CLOSED";
})(RxStompState || (RxStompState = {}));
//#endregion
//#region node_modules/@stomp/rx-stomp/esm6/rx-stomp.js
/**
* Main RxJS-friendly STOMP client for browsers and Node.js.
*
* RxStomp wraps {@link Client} from @stomp/stompjs and exposes key interactions
* (connection lifecycle, subscriptions, frames, and errors) as RxJS streams.
*
* What RxStomp adds:
* - Simple, observable-based API for consuming messages.
* - Connection lifecycle as BehaviorSubjects/Observables for easy UI binding.
* - Transparent reconnection support (configurable through {@link RxStompConfig}).
* - Convenience helpers for receipts and server headers.
*
* Typical lifecycle:
* - Instantiate: `const rxStomp = new RxStomp();`
* - Configure: `rxStomp.configure({...});`
* - Activate: `rxStomp.activate();`
* - Consume: `rxStomp.watch({ destination: '/topic/foo' }).subscribe(...)`
* - Publish: `rxStomp.publish({ destination: '/topic/foo', body: '...' })`
* - Deactivate when done: `await rxStomp.deactivate();`
*
* Notes:
* - Except for `beforeConnect`, all callbacks from @stomp/stompjs are exposed
*   as RxJS Subjects/Observables here.
* - RxStomp tries to transparently handle connection failures.
*
* Part of `@stomp/rx-stomp`.
*/
var RxStomp = class {
	/**
	* Instance of actual
	* [@stomp/stompjs]{@link https://github.com/stomp-js/stompjs}
	* {@link Client}.
	*
	* **Be careful in calling methods on it directly - you may get unintended consequences.**
	*/
	get stompClient() {
		return this._stompClient;
	}
	/**
	* Constructor
	*
	* @param stompClient Optional existing {@link Client} to wrap. If omitted, a new instance
	* will be created internally.
	*
	* Tip: Injecting a pre-configured Client is useful for advanced customization or testing.
	*/
	constructor(stompClient) {
		/**
		* Internal array to hold locally queued messages when STOMP broker is not connected.
		*/
		this._queuedMessages = [];
		this._stompClient = stompClient ? stompClient : new import_stomp_umd.Client();
		const noOp = () => {};
		this._beforeConnect = noOp;
		this._correlateErrors = () => void 0;
		this._debug = noOp;
		this._connectionStatePre$ = new BehaviorSubject(RxStompState.CLOSED);
		this._connectedPre$ = this._connectionStatePre$.pipe(filter((currentState) => {
			return currentState === RxStompState.OPEN;
		}));
		this.connectionState$ = new BehaviorSubject(RxStompState.CLOSED);
		this.connected$ = this.connectionState$.pipe(filter((currentState) => {
			return currentState === RxStompState.OPEN;
		}));
		this.connected$.subscribe(() => {
			this._sendQueuedMessages();
		});
		this._serverHeadersBehaviourSubject$ = new BehaviorSubject(null);
		this.serverHeaders$ = this._serverHeadersBehaviourSubject$.pipe(filter((headers) => {
			return headers !== null;
		}));
		this.stompErrors$ = new Subject();
		this.unhandledMessage$ = new Subject();
		this.unhandledReceipts$ = new Subject();
		this.unhandledFrame$ = new Subject();
		this.webSocketErrors$ = new Subject();
	}
	/**
	* Apply configuration to the underlying STOMP client.
	*
	* - Safe to call multiple times; each call merges with existing configuration.
	* - `beforeConnect` and `correlateErrors` are handled by RxStomp and removed
	*   from the object passed to the underlying {@link Client}.
	* - Unless otherwise documented by @stomp/stompjs, most options take effect
	*   on the next (re)connection.
	*
	* Example:
	* ```typescript
	* const rxStomp = new RxStomp();
	* rxStomp.configure({
	*   brokerURL: 'ws://127.0.0.1:15674/ws',
	*   connectHeaders: {
	*     login: 'guest',
	*     passcode: 'guest'
	*     },
	*   heartbeatIncoming: 0,
	*   heartbeatOutgoing: 20000,
	*   reconnectDelay: 200,
	*   debug: (msg) => console.log(new Date(), msg),
	* });
	* rxStomp.activate();
	* ```
	*
	* Maps to: [Client#configure]{@link Client#configure}.
	*/
	configure(rxStompConfig) {
		const stompConfig = Object.assign({}, rxStompConfig);
		if (stompConfig.beforeConnect) {
			this._beforeConnect = stompConfig.beforeConnect;
			delete stompConfig.beforeConnect;
		}
		if (stompConfig.correlateErrors) {
			this._correlateErrors = stompConfig.correlateErrors;
			delete stompConfig.correlateErrors;
		}
		this._stompClient.configure(stompConfig);
		if (stompConfig.debug) this._debug = stompConfig.debug;
	}
	/**
	* Activate the client and initiate connection attempts.
	*
	* - Emits `CONNECTING` followed by `OPEN` on successful connect.
	* - Automatically reconnects, according to configuration.
	*
	* To stop auto-reconnect and close the connection, call {@link deactivate}.
	*
	* Maps to: [Client#activate]{@link Client#activate}.
	*/
	activate() {
		this._stompClient.configure({
			beforeConnect: async () => {
				this._changeState(RxStompState.CONNECTING);
				await this._beforeConnect(this);
			},
			onConnect: (frame) => {
				this._serverHeadersBehaviourSubject$.next(frame.headers);
				this._changeState(RxStompState.OPEN);
			},
			onStompError: (frame) => {
				this.stompErrors$.next(frame);
			},
			onWebSocketClose: () => {
				this._changeState(RxStompState.CLOSED);
			},
			onUnhandledMessage: (message) => {
				this.unhandledMessage$.next(message);
			},
			onUnhandledReceipt: (frame) => {
				this.unhandledReceipts$.next(frame);
			},
			onUnhandledFrame: (frame) => {
				this.unhandledFrame$.next(frame);
			},
			onWebSocketError: (evt) => {
				this.webSocketErrors$.next(evt);
			}
		});
		this._stompClient.activate();
	}
	/**
	* Gracefully disconnect (if connected) and stop auto-reconnect.
	*
	* Behavior:
	* - Emits `CLOSING` then `CLOSED`.
	* - If no active WebSocket exists, resolves immediately.
	* - If a WebSocket is active, resolves after it is properly closed.
	*
	* Experimental:
	* - `options.force === true` immediately discards the underlying connection.
	*   See [Client#deactivate]{@link Client#deactivate}.
	*
	* You can call {@link activate} again after awa.
	*
	* Maps to: [Client#deactivate]{@link Client#deactivate}.
	*/
	async deactivate(options = {}) {
		this._changeState(RxStompState.CLOSING);
		await this._stompClient.deactivate(options);
		this._changeState(RxStompState.CLOSED);
	}
	/**
	* Whether the broker connection is currently OPEN.
	*
	* Equivalent to checking `connectionState$.value === RxStompState.OPEN`.
	*/
	connected() {
		return this.connectionState$.getValue() === RxStompState.OPEN;
	}
	/**
	* True if the client is ACTIVE — i.e., connected or attempting reconnection.
	*
	* Maps to: [Client#active]{@link Client#active}.
	*/
	get active() {
		return this.stompClient.active;
	}
	/**
	* Publish a message to a destination on the broker.
	*
	* Destination semantics and supported headers are broker-specific; consult your broker’s docs
	* for naming conventions (queues, topics, exchanges) and header support.
	*
	* Payload
	* - Text: provide `body` as a string. Convert non-strings yourself (e.g., JSON.stringify).
	* - Binary: provide `binaryBody` as a Uint8Array and set an appropriate `content-type` header.
	*   Some brokers may require explicit configuration for binary frames.
	*
	* Frame sizing and content-length
	* - For text messages, a `content-length` header is added by default.
	*   Set `skipContentLengthHeader: true` to omit it for text frames.
	* - For binary messages, `content-length` is always included.
	*
	* Caution
	* - If a message body contains NULL octets and the `content-length` header is omitted,
	*   many brokers will report an error and disconnect.
	*
	* Offline/queueing behavior
	* - If not connected, messages are queued locally and sent upon reconnection.
	* - To disable this behavior, set `retryIfDisconnected: false` in the parameters.
	*   In that case, this method throws if it cannot send immediately.
	*
	* Related
	* - Broker acknowledgment (receipt) can be tracked using `receipt` headers together with {@link asyncReceipt}.
	*
	* Maps to: [Client#publish]{@link Client#publish}
	*
	* See: {@link IRxStompPublishParams} and {@link IPublishParams}
	*
	* Examples:
	* ```javascript
	* // Text with custom headers
	* rxStomp.publish({ destination: "/queue/test", headers: { priority: 9 }, body: "Hello, STOMP" });
	*
	* // Minimal (destination is required)
	* rxStomp.publish({ destination: "/queue/test", body: "Hello, STOMP" });
	*
	* // Skip content-length header for text
	* rxStomp.publish({ destination: "/queue/test", body: "Hello, STOMP", skipContentLengthHeader: true });
	*
	* // Binary payload
	* const binaryData = generateBinaryData(); // Uint8Array
	* rxStomp.publish({
	*   destination: "/topic/special",
	*   binaryBody: binaryData,
	*   headers: { "content-type": "application/octet-stream" }
	* });
	* ```
	*/
	publish(parameters) {
		const shouldRetry = parameters.retryIfDisconnected == null ? true : parameters.retryIfDisconnected;
		if (this.connected()) this._stompClient.publish(parameters);
		else if (shouldRetry) {
			this._debug(`Not connected, queueing`);
			this._queuedMessages.push(parameters);
		} else throw new Error("Cannot publish while broker is not connected");
	}
	/** It will send queued messages. */
	_sendQueuedMessages() {
		const queuedMessages = this._queuedMessages;
		this._queuedMessages = [];
		if (queuedMessages.length === 0) return;
		this._debug(`Will try sending  ${queuedMessages.length} queued message(s)`);
		for (const queuedMessage of queuedMessages) {
			this._debug(`Attempting to send ${queuedMessage}`);
			this.publish(queuedMessage);
		}
	}
	watch(opts, headers = {}) {
		const defaults = {
			subHeaders: {},
			unsubHeaders: {},
			subscribeOnlyOnce: false
		};
		let params;
		if (typeof opts === "string") params = Object.assign({}, defaults, {
			destination: opts,
			subHeaders: headers
		});
		else params = Object.assign({}, defaults, opts);
		this._debug(`Request to subscribe ${params.destination}`);
		/**
		* Important - convert it to hot Observable - otherwise, if the user code subscribes
		* to this observable twice, it will subscribe twice to Stomp broker. (This was happening in the current example).
		* A long but good explanatory article at https://medium.com/@benlesh/hot-vs-cold-observables-f8094ed53339
		*/
		return Observable.create((messages) => {
			let stompSubscription;
			let stompConnectedSubscription;
			let connectedPre$ = this._connectedPre$;
			if (params.subscribeOnlyOnce) connectedPre$ = connectedPre$.pipe(take(1));
			const stompErrorsSubscription = this.stompErrors$.subscribe((error) => {
				if (this._correlateErrors(error) === params.destination) messages.error(error);
			});
			stompConnectedSubscription = connectedPre$.subscribe(() => {
				this._debug(`Will subscribe to ${params.destination}`);
				let subHeaders = params.subHeaders;
				if (typeof subHeaders === "function") subHeaders = subHeaders();
				stompSubscription = this._stompClient.subscribe(params.destination, (message) => {
					messages.next(message);
				}, subHeaders);
			});
			return () => {
				this._debug(`Stop watching connection state (for ${params.destination})`);
				stompConnectedSubscription.unsubscribe();
				stompErrorsSubscription.unsubscribe();
				if (this.connected()) {
					this._debug(`Will unsubscribe from ${params.destination} at Stomp`);
					let unsubHeaders = params.unsubHeaders;
					if (typeof unsubHeaders === "function") unsubHeaders = unsubHeaders();
					stompSubscription.unsubscribe(unsubHeaders);
				} else this._debug(`Stomp not connected, no need to unsubscribe from ${params.destination} at Stomp`);
			};
		}).pipe(share());
	}
	/**
	* **Deprecated** Please use {@link asyncReceipt}.
	*/
	watchForReceipt(receiptId, callback) {
		this._stompClient.watchForReceipt(receiptId, callback);
	}
	/**
	* Wait for a broker RECEIPT matching the provided receipt-id.
	*
	* How it works
	* - To request an acknowledgment for an operation (e.g., publish, subscribe, unsubscribe),
	*   include a `receipt` header in that operation with a unique value.
	* - A compliant broker will respond with a `RECEIPT` frame whose `receipt-id` header equals
	*   the value you sent in the `receipt` header.
	* - This method returns a Promise that resolves with the matching {@link IFrame} when the
	*   corresponding `RECEIPT` arrives.
	*
	* Receipt identifiers
	* - Must be unique per request; generating a UUID or monotonic sequence is typical.
	*
	* Notes
	* - The Promise resolves once for the first matching receipt and then completes.
	* - No timeout is enforced by default; to add one, wrap with your own timeout logic (e.g., Promise.race).
	*
	* Example:
	* ```javascript
	* // Publish with receipt tracking
	* const receiptId = randomText();
	* rxStomp.publish({
	*   destination: "/topic/special",
	*   headers: { receipt: receiptId },
	*   body: msg
	* });
	* const receiptFrame = await rxStomp.asyncReceipt(receiptId); // resolves with the RECEIPT frame
	* ```
	*
	* Maps to: [Client#watchForReceipt]{@link Client#watchForReceipt}
	*/
	asyncReceipt(receiptId) {
		return firstValueFrom(this.unhandledReceipts$.pipe(filter((frame) => frame.headers["receipt-id"] === receiptId)));
	}
	_changeState(state) {
		this._connectionStatePre$.next(state);
		this.connectionState$.next(state);
	}
};
//#endregion
//#region src/stores/StompStore.tsx
var rxStompConfig = {
	brokerURL: "/websocket/stomp",
	connectHeaders: {
		login: "",
		passcode: ""
	},
	heartbeatIncoming: 1e3,
	heartbeatOutgoing: 1e3,
	reconnectDelay: 1e3,
	stompVersions: new import_stomp_umd.Versions([])
};
var useStompStore = create((set, get) => ({
	status: "none",
	stompClient: null,
	mqttmanager_warnings: [],
	connect: () => {
		set({ status: "connecting" });
		if (!get().stompClient) set({ stompClient: new RxStomp() });
		get().stompClient?.configure(rxStompConfig);
		get().stompClient?.activate();
		get().stompClient?.connected$.subscribe(() => {
			set({ status: "connected" });
			RemoveNotification("stomp_disconnected");
		});
		get().stompClient?.webSocketErrors$.subscribe(() => {
			set({ status: "disconnected" });
			Notify({
				message: "Disconnected from MQTTManager.",
				level: "error",
				toast_id: "stomp_disconnected"
			});
		});
		get().subscribe("mqttmanager/warnings", (message) => {
			const data = JSON.parse(message.body);
			const warnings = data.warnings;
			const resolvedWarnings = get().mqttmanager_warnings.filter((warning) => {
				return !warnings.some((new_warning) => new_warning.text == warning.text);
			});
			for (const warning of resolvedWarnings) RemoveNotification(warning.text);
			for (const warning of warnings) Notify({
				message: warning.text,
				level: warning.level,
				toast_id: warning.text
			});
			set({ mqttmanager_warnings: data.warnings });
		});
	},
	subscribe: (topic, callback) => {
		if (!get()._subscriptions.has(topic)) {
			const stompClient = get().stompClient;
			if (stompClient) {
				const subscription = stompClient.watch(topic).subscribe((message) => {
					if (!get()._subscriptions.has(topic)) get()._subscriptions.set(topic, {
						topic,
						callbacks: [],
						subscription
					});
					const topicData = get()._subscriptions.get(topic);
					if (topicData) {
						topicData.last_message = message;
						for (const cb of topicData.callbacks) cb(message);
					}
				});
				get()._subscriptions.set(topic, {
					topic,
					callbacks: [callback],
					subscription
				});
			} else console.error("Tried to subscribe to ", topic, " but stompClient is null");
		} else {
			const subscription = get()._subscriptions.get(topic);
			if (subscription) {
				subscription.callbacks.push(callback);
				if (subscription.last_message) callback(subscription.last_message);
			}
		}
	},
	unsubscribe: (topic, callback) => {
		if (get()._subscriptions.has(topic)) {
			const subscription = get()._subscriptions.get(topic);
			if (subscription) {
				const index = subscription.callbacks.indexOf(callback);
				if (index !== -1) {
					subscription.callbacks.splice(index, 1);
					if (subscription.callbacks.length === 0) {
						subscription.subscription?.unsubscribe();
						get()._subscriptions.delete(topic);
					}
				}
			}
		}
	},
	send: (topic, body) => {
		get().stompClient?.publish({
			destination: topic,
			body
		});
	},
	_subscriptions: /* @__PURE__ */ new Map()
}));
//#endregion
export { useStompStore as t };
