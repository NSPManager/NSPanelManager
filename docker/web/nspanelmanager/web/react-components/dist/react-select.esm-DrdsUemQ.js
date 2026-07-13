import { c as __toESM, i as __commonJSMin, n as require_react_dom, r as require_react } from "./main-afCuO7kO.js";
//#region node_modules/@babel/runtime/helpers/esm/typeof.js
function _typeof(o) {
	"@babel/helpers - typeof";
	return _typeof = "function" == typeof Symbol && "symbol" == typeof Symbol.iterator ? function(o) {
		return typeof o;
	} : function(o) {
		return o && "function" == typeof Symbol && o.constructor === Symbol && o !== Symbol.prototype ? "symbol" : typeof o;
	}, _typeof(o);
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/toPrimitive.js
function toPrimitive(t, r) {
	if ("object" != _typeof(t) || !t) return t;
	var e = t[Symbol.toPrimitive];
	if (void 0 !== e) {
		var i = e.call(t, r || "default");
		if ("object" != _typeof(i)) return i;
		throw new TypeError("@@toPrimitive must return a primitive value.");
	}
	return ("string" === r ? String : Number)(t);
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/toPropertyKey.js
function toPropertyKey(t) {
	var i = toPrimitive(t, "string");
	return "symbol" == _typeof(i) ? i : i + "";
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/defineProperty.js
function _defineProperty(e, r, t) {
	return (r = toPropertyKey(r)) in e ? Object.defineProperty(e, r, {
		value: t,
		enumerable: !0,
		configurable: !0,
		writable: !0
	}) : e[r] = t, e;
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/objectSpread2.js
function ownKeys(e, r) {
	var t = Object.keys(e);
	if (Object.getOwnPropertySymbols) {
		var o = Object.getOwnPropertySymbols(e);
		r && (o = o.filter(function(r) {
			return Object.getOwnPropertyDescriptor(e, r).enumerable;
		})), t.push.apply(t, o);
	}
	return t;
}
function _objectSpread2(e) {
	for (var r = 1; r < arguments.length; r++) {
		var t = null != arguments[r] ? arguments[r] : {};
		r % 2 ? ownKeys(Object(t), !0).forEach(function(r) {
			_defineProperty(e, r, t[r]);
		}) : Object.getOwnPropertyDescriptors ? Object.defineProperties(e, Object.getOwnPropertyDescriptors(t)) : ownKeys(Object(t)).forEach(function(r) {
			Object.defineProperty(e, r, Object.getOwnPropertyDescriptor(t, r));
		});
	}
	return e;
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/arrayWithHoles.js
function _arrayWithHoles(r) {
	if (Array.isArray(r)) return r;
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/iterableToArrayLimit.js
function _iterableToArrayLimit(r, l) {
	var t = null == r ? null : "undefined" != typeof Symbol && r[Symbol.iterator] || r["@@iterator"];
	if (null != t) {
		var e, n, i, u, a = [], f = !0, o = !1;
		try {
			if (i = (t = t.call(r)).next, 0 === l) {
				if (Object(t) !== t) return;
				f = !1;
			} else for (; !(f = (e = i.call(t)).done) && (a.push(e.value), a.length !== l); f = !0);
		} catch (r) {
			o = !0, n = r;
		} finally {
			try {
				if (!f && null != t["return"] && (u = t["return"](), Object(u) !== u)) return;
			} finally {
				if (o) throw n;
			}
		}
		return a;
	}
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/arrayLikeToArray.js
function _arrayLikeToArray(r, a) {
	(null == a || a > r.length) && (a = r.length);
	for (var e = 0, n = Array(a); e < a; e++) n[e] = r[e];
	return n;
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/unsupportedIterableToArray.js
function _unsupportedIterableToArray(r, a) {
	if (r) {
		if ("string" == typeof r) return _arrayLikeToArray(r, a);
		var t = {}.toString.call(r).slice(8, -1);
		return "Object" === t && r.constructor && (t = r.constructor.name), "Map" === t || "Set" === t ? Array.from(r) : "Arguments" === t || /^(?:Ui|I)nt(?:8|16|32)(?:Clamped)?Array$/.test(t) ? _arrayLikeToArray(r, a) : void 0;
	}
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/nonIterableRest.js
function _nonIterableRest() {
	throw new TypeError("Invalid attempt to destructure non-iterable instance.\nIn order to be iterable, non-array objects must have a [Symbol.iterator]() method.");
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/slicedToArray.js
function _slicedToArray(r, e) {
	return _arrayWithHoles(r) || _iterableToArrayLimit(r, e) || _unsupportedIterableToArray(r, e) || _nonIterableRest();
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/objectWithoutPropertiesLoose.js
function _objectWithoutPropertiesLoose(r, e) {
	if (null == r) return {};
	var t = {};
	for (var n in r) if ({}.hasOwnProperty.call(r, n)) {
		if (-1 !== e.indexOf(n)) continue;
		t[n] = r[n];
	}
	return t;
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/objectWithoutProperties.js
function _objectWithoutProperties(e, t) {
	if (null == e) return {};
	var o, r, i = _objectWithoutPropertiesLoose(e, t);
	if (Object.getOwnPropertySymbols) {
		var n = Object.getOwnPropertySymbols(e);
		for (r = 0; r < n.length; r++) o = n[r], -1 === t.indexOf(o) && {}.propertyIsEnumerable.call(e, o) && (i[o] = e[o]);
	}
	return i;
}
//#endregion
//#region node_modules/react-select/dist/useStateManager-7e1e8489.esm.js
var import_react = /* @__PURE__ */ __toESM(require_react());
var _excluded$6 = [
	"defaultInputValue",
	"defaultMenuIsOpen",
	"defaultValue",
	"inputValue",
	"menuIsOpen",
	"onChange",
	"onInputChange",
	"onMenuClose",
	"onMenuOpen",
	"value"
];
function useStateManager(_ref) {
	var _ref$defaultInputValu = _ref.defaultInputValue, defaultInputValue = _ref$defaultInputValu === void 0 ? "" : _ref$defaultInputValu, _ref$defaultMenuIsOpe = _ref.defaultMenuIsOpen, defaultMenuIsOpen = _ref$defaultMenuIsOpe === void 0 ? false : _ref$defaultMenuIsOpe, _ref$defaultValue = _ref.defaultValue, defaultValue = _ref$defaultValue === void 0 ? null : _ref$defaultValue, propsInputValue = _ref.inputValue, propsMenuIsOpen = _ref.menuIsOpen, propsOnChange = _ref.onChange, propsOnInputChange = _ref.onInputChange, propsOnMenuClose = _ref.onMenuClose, propsOnMenuOpen = _ref.onMenuOpen, propsValue = _ref.value, restSelectProps = _objectWithoutProperties(_ref, _excluded$6);
	var _useState2 = _slicedToArray((0, import_react.useState)(propsInputValue !== void 0 ? propsInputValue : defaultInputValue), 2), stateInputValue = _useState2[0], setStateInputValue = _useState2[1];
	var _useState4 = _slicedToArray((0, import_react.useState)(propsMenuIsOpen !== void 0 ? propsMenuIsOpen : defaultMenuIsOpen), 2), stateMenuIsOpen = _useState4[0], setStateMenuIsOpen = _useState4[1];
	var _useState6 = _slicedToArray((0, import_react.useState)(propsValue !== void 0 ? propsValue : defaultValue), 2), stateValue = _useState6[0], setStateValue = _useState6[1];
	var onChange = (0, import_react.useCallback)(function(value, actionMeta) {
		if (typeof propsOnChange === "function") propsOnChange(value, actionMeta);
		setStateValue(value);
	}, [propsOnChange]);
	var onInputChange = (0, import_react.useCallback)(function(value, actionMeta) {
		var newValue;
		if (typeof propsOnInputChange === "function") newValue = propsOnInputChange(value, actionMeta);
		setStateInputValue(newValue !== void 0 ? newValue : value);
	}, [propsOnInputChange]);
	var onMenuOpen = (0, import_react.useCallback)(function() {
		if (typeof propsOnMenuOpen === "function") propsOnMenuOpen();
		setStateMenuIsOpen(true);
	}, [propsOnMenuOpen]);
	var onMenuClose = (0, import_react.useCallback)(function() {
		if (typeof propsOnMenuClose === "function") propsOnMenuClose();
		setStateMenuIsOpen(false);
	}, [propsOnMenuClose]);
	var inputValue = propsInputValue !== void 0 ? propsInputValue : stateInputValue;
	var menuIsOpen = propsMenuIsOpen !== void 0 ? propsMenuIsOpen : stateMenuIsOpen;
	var value = propsValue !== void 0 ? propsValue : stateValue;
	return _objectSpread2(_objectSpread2({}, restSelectProps), {}, {
		inputValue,
		menuIsOpen,
		onChange,
		onInputChange,
		onMenuClose,
		onMenuOpen,
		value
	});
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/extends.js
function _extends() {
	return _extends = Object.assign ? Object.assign.bind() : function(n) {
		for (var e = 1; e < arguments.length; e++) {
			var t = arguments[e];
			for (var r in t) ({}).hasOwnProperty.call(t, r) && (n[r] = t[r]);
		}
		return n;
	}, _extends.apply(null, arguments);
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/classCallCheck.js
function _classCallCheck(a, n) {
	if (!(a instanceof n)) throw new TypeError("Cannot call a class as a function");
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/createClass.js
function _defineProperties(e, r) {
	for (var t = 0; t < r.length; t++) {
		var o = r[t];
		o.enumerable = o.enumerable || !1, o.configurable = !0, "value" in o && (o.writable = !0), Object.defineProperty(e, toPropertyKey(o.key), o);
	}
}
function _createClass(e, r, t) {
	return r && _defineProperties(e.prototype, r), t && _defineProperties(e, t), Object.defineProperty(e, "prototype", { writable: !1 }), e;
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/setPrototypeOf.js
function _setPrototypeOf(t, e) {
	return _setPrototypeOf = Object.setPrototypeOf ? Object.setPrototypeOf.bind() : function(t, e) {
		return t.__proto__ = e, t;
	}, _setPrototypeOf(t, e);
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/inherits.js
function _inherits(t, e) {
	if ("function" != typeof e && null !== e) throw new TypeError("Super expression must either be null or a function");
	t.prototype = Object.create(e && e.prototype, { constructor: {
		value: t,
		writable: !0,
		configurable: !0
	} }), Object.defineProperty(t, "prototype", { writable: !1 }), e && _setPrototypeOf(t, e);
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/getPrototypeOf.js
function _getPrototypeOf(t) {
	return _getPrototypeOf = Object.setPrototypeOf ? Object.getPrototypeOf.bind() : function(t) {
		return t.__proto__ || Object.getPrototypeOf(t);
	}, _getPrototypeOf(t);
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/isNativeReflectConstruct.js
function _isNativeReflectConstruct() {
	try {
		var t = !Boolean.prototype.valueOf.call(Reflect.construct(Boolean, [], function() {}));
	} catch (t) {}
	return (_isNativeReflectConstruct = function _isNativeReflectConstruct() {
		return !!t;
	})();
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/assertThisInitialized.js
function _assertThisInitialized(e) {
	if (void 0 === e) throw new ReferenceError("this hasn't been initialised - super() hasn't been called");
	return e;
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/possibleConstructorReturn.js
function _possibleConstructorReturn(t, e) {
	if (e && ("object" == _typeof(e) || "function" == typeof e)) return e;
	if (void 0 !== e) throw new TypeError("Derived constructors may only return object or undefined");
	return _assertThisInitialized(t);
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/createSuper.js
function _createSuper(t) {
	var r = _isNativeReflectConstruct();
	return function() {
		var e, o = _getPrototypeOf(t);
		if (r) {
			var s = _getPrototypeOf(this).constructor;
			e = Reflect.construct(o, arguments, s);
		} else e = o.apply(this, arguments);
		return _possibleConstructorReturn(this, e);
	};
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/arrayWithoutHoles.js
function _arrayWithoutHoles(r) {
	if (Array.isArray(r)) return _arrayLikeToArray(r);
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/iterableToArray.js
function _iterableToArray(r) {
	if ("undefined" != typeof Symbol && null != r[Symbol.iterator] || null != r["@@iterator"]) return Array.from(r);
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/nonIterableSpread.js
function _nonIterableSpread() {
	throw new TypeError("Invalid attempt to spread non-iterable instance.\nIn order to be iterable, non-array objects must have a [Symbol.iterator]() method.");
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/toConsumableArray.js
function _toConsumableArray(r) {
	return _arrayWithoutHoles(r) || _iterableToArray(r) || _unsupportedIterableToArray(r) || _nonIterableSpread();
}
//#endregion
//#region node_modules/@emotion/sheet/dist/emotion-sheet.esm.js
function sheetForTag(tag) {
	if (tag.sheet) return tag.sheet;
	/* istanbul ignore next */
	for (var i = 0; i < document.styleSheets.length; i++) if (document.styleSheets[i].ownerNode === tag) return document.styleSheets[i];
}
function createStyleElement(options) {
	var tag = document.createElement("style");
	tag.setAttribute("data-emotion", options.key);
	if (options.nonce !== void 0) tag.setAttribute("nonce", options.nonce);
	tag.appendChild(document.createTextNode(""));
	tag.setAttribute("data-s", "");
	return tag;
}
var StyleSheet = /*#__PURE__*/ function() {
	function StyleSheet(options) {
		var _this = this;
		this._insertTag = function(tag) {
			var before;
			if (_this.tags.length === 0) if (_this.insertionPoint) before = _this.insertionPoint.nextSibling;
			else if (_this.prepend) before = _this.container.firstChild;
			else before = _this.before;
			else before = _this.tags[_this.tags.length - 1].nextSibling;
			_this.container.insertBefore(tag, before);
			_this.tags.push(tag);
		};
		this.isSpeedy = options.speedy === void 0 ? true : options.speedy;
		this.tags = [];
		this.ctr = 0;
		this.nonce = options.nonce;
		this.key = options.key;
		this.container = options.container;
		this.prepend = options.prepend;
		this.insertionPoint = options.insertionPoint;
		this.before = null;
	}
	var _proto = StyleSheet.prototype;
	_proto.hydrate = function hydrate(nodes) {
		nodes.forEach(this._insertTag);
	};
	_proto.insert = function insert(rule) {
		if (this.ctr % (this.isSpeedy ? 65e3 : 1) === 0) this._insertTag(createStyleElement(this));
		var tag = this.tags[this.tags.length - 1];
		if (this.isSpeedy) {
			var sheet = sheetForTag(tag);
			try {
				sheet.insertRule(rule, sheet.cssRules.length);
			} catch (e) {}
		} else tag.appendChild(document.createTextNode(rule));
		this.ctr++;
	};
	_proto.flush = function flush() {
		this.tags.forEach(function(tag) {
			var _tag$parentNode;
			return (_tag$parentNode = tag.parentNode) == null ? void 0 : _tag$parentNode.removeChild(tag);
		});
		this.tags = [];
		this.ctr = 0;
	};
	return StyleSheet;
}();
//#endregion
//#region node_modules/stylis/src/Enum.js
var MS = "-ms-";
var MOZ = "-moz-";
var WEBKIT = "-webkit-";
var COMMENT = "comm";
var RULESET = "rule";
var DECLARATION = "decl";
var IMPORT = "@import";
var KEYFRAMES = "@keyframes";
var LAYER = "@layer";
//#endregion
//#region node_modules/stylis/src/Utility.js
/**
* @param {number}
* @return {number}
*/
var abs = Math.abs;
/**
* @param {number}
* @return {string}
*/
var from = String.fromCharCode;
/**
* @param {object}
* @return {object}
*/
var assign = Object.assign;
/**
* @param {string} value
* @param {number} length
* @return {number}
*/
function hash(value, length) {
	return charat(value, 0) ^ 45 ? (((length << 2 ^ charat(value, 0)) << 2 ^ charat(value, 1)) << 2 ^ charat(value, 2)) << 2 ^ charat(value, 3) : 0;
}
/**
* @param {string} value
* @return {string}
*/
function trim(value) {
	return value.trim();
}
/**
* @param {string} value
* @param {RegExp} pattern
* @return {string?}
*/
function match(value, pattern) {
	return (value = pattern.exec(value)) ? value[0] : value;
}
/**
* @param {string} value
* @param {(string|RegExp)} pattern
* @param {string} replacement
* @return {string}
*/
function replace(value, pattern, replacement) {
	return value.replace(pattern, replacement);
}
/**
* @param {string} value
* @param {string} search
* @return {number}
*/
function indexof(value, search) {
	return value.indexOf(search);
}
/**
* @param {string} value
* @param {number} index
* @return {number}
*/
function charat(value, index) {
	return value.charCodeAt(index) | 0;
}
/**
* @param {string} value
* @param {number} begin
* @param {number} end
* @return {string}
*/
function substr(value, begin, end) {
	return value.slice(begin, end);
}
/**
* @param {string} value
* @return {number}
*/
function strlen(value) {
	return value.length;
}
/**
* @param {any[]} value
* @return {number}
*/
function sizeof(value) {
	return value.length;
}
/**
* @param {any} value
* @param {any[]} array
* @return {any}
*/
function append(value, array) {
	return array.push(value), value;
}
/**
* @param {string[]} array
* @param {function} callback
* @return {string}
*/
function combine(array, callback) {
	return array.map(callback).join("");
}
//#endregion
//#region node_modules/stylis/src/Tokenizer.js
var line = 1;
var column = 1;
var length = 0;
var position = 0;
var character = 0;
var characters = "";
/**
* @param {string} value
* @param {object | null} root
* @param {object | null} parent
* @param {string} type
* @param {string[] | string} props
* @param {object[] | string} children
* @param {number} length
*/
function node(value, root, parent, type, props, children, length) {
	return {
		value,
		root,
		parent,
		type,
		props,
		children,
		line,
		column,
		length,
		return: ""
	};
}
/**
* @param {object} root
* @param {object} props
* @return {object}
*/
function copy(root, props) {
	return assign(node("", null, null, "", null, null, 0), root, { length: -root.length }, props);
}
/**
* @return {number}
*/
function char() {
	return character;
}
/**
* @return {number}
*/
function prev() {
	character = position > 0 ? charat(characters, --position) : 0;
	if (column--, character === 10) column = 1, line--;
	return character;
}
/**
* @return {number}
*/
function next() {
	character = position < length ? charat(characters, position++) : 0;
	if (column++, character === 10) column = 1, line++;
	return character;
}
/**
* @return {number}
*/
function peek() {
	return charat(characters, position);
}
/**
* @return {number}
*/
function caret() {
	return position;
}
/**
* @param {number} begin
* @param {number} end
* @return {string}
*/
function slice(begin, end) {
	return substr(characters, begin, end);
}
/**
* @param {number} type
* @return {number}
*/
function token(type) {
	switch (type) {
		case 0:
		case 9:
		case 10:
		case 13:
		case 32: return 5;
		case 33:
		case 43:
		case 44:
		case 47:
		case 62:
		case 64:
		case 126:
		case 59:
		case 123:
		case 125: return 4;
		case 58: return 3;
		case 34:
		case 39:
		case 40:
		case 91: return 2;
		case 41:
		case 93: return 1;
	}
	return 0;
}
/**
* @param {string} value
* @return {any[]}
*/
function alloc(value) {
	return line = column = 1, length = strlen(characters = value), position = 0, [];
}
/**
* @param {any} value
* @return {any}
*/
function dealloc(value) {
	return characters = "", value;
}
/**
* @param {number} type
* @return {string}
*/
function delimit(type) {
	return trim(slice(position - 1, delimiter(type === 91 ? type + 2 : type === 40 ? type + 1 : type)));
}
/**
* @param {number} type
* @return {string}
*/
function whitespace(type) {
	while (character = peek()) if (character < 33) next();
	else break;
	return token(type) > 2 || token(character) > 3 ? "" : " ";
}
/**
* @param {number} index
* @param {number} count
* @return {string}
*/
function escaping(index, count) {
	while (--count && next()) if (character < 48 || character > 102 || character > 57 && character < 65 || character > 70 && character < 97) break;
	return slice(index, caret() + (count < 6 && peek() == 32 && next() == 32));
}
/**
* @param {number} type
* @return {number}
*/
function delimiter(type) {
	while (next()) switch (character) {
		case type: return position;
		case 34:
		case 39:
			if (type !== 34 && type !== 39) delimiter(character);
			break;
		case 40:
			if (type === 41) delimiter(type);
			break;
		case 92:
			next();
			break;
	}
	return position;
}
/**
* @param {number} type
* @param {number} index
* @return {number}
*/
function commenter(type, index) {
	while (next()) if (type + character === 57) break;
	else if (type + character === 84 && peek() === 47) break;
	return "/*" + slice(index, position - 1) + "*" + from(type === 47 ? type : next());
}
/**
* @param {number} index
* @return {string}
*/
function identifier(index) {
	while (!token(peek())) next();
	return slice(index, position);
}
//#endregion
//#region node_modules/stylis/src/Parser.js
/**
* @param {string} value
* @return {object[]}
*/
function compile(value) {
	return dealloc(parse("", null, null, null, [""], value = alloc(value), 0, [0], value));
}
/**
* @param {string} value
* @param {object} root
* @param {object?} parent
* @param {string[]} rule
* @param {string[]} rules
* @param {string[]} rulesets
* @param {number[]} pseudo
* @param {number[]} points
* @param {string[]} declarations
* @return {object}
*/
function parse(value, root, parent, rule, rules, rulesets, pseudo, points, declarations) {
	var index = 0;
	var offset = 0;
	var length = pseudo;
	var atrule = 0;
	var property = 0;
	var previous = 0;
	var variable = 1;
	var scanning = 1;
	var ampersand = 1;
	var character = 0;
	var type = "";
	var props = rules;
	var children = rulesets;
	var reference = rule;
	var characters = type;
	while (scanning) switch (previous = character, character = next()) {
		case 40: if (previous != 108 && charat(characters, length - 1) == 58) {
			if (indexof(characters += replace(delimit(character), "&", "&\f"), "&\f") != -1) ampersand = -1;
			break;
		}
		case 34:
		case 39:
		case 91:
			characters += delimit(character);
			break;
		case 9:
		case 10:
		case 13:
		case 32:
			characters += whitespace(previous);
			break;
		case 92:
			characters += escaping(caret() - 1, 7);
			continue;
		case 47:
			switch (peek()) {
				case 42:
				case 47:
					append(comment(commenter(next(), caret()), root, parent), declarations);
					break;
				default: characters += "/";
			}
			break;
		case 123 * variable: points[index++] = strlen(characters) * ampersand;
		case 125 * variable:
		case 59:
		case 0:
			switch (character) {
				case 0:
				case 125: scanning = 0;
				case 59 + offset:
					if (ampersand == -1) characters = replace(characters, /\f/g, "");
					if (property > 0 && strlen(characters) - length) append(property > 32 ? declaration(characters + ";", rule, parent, length - 1) : declaration(replace(characters, " ", "") + ";", rule, parent, length - 2), declarations);
					break;
				case 59: characters += ";";
				default:
					append(reference = ruleset(characters, root, parent, index, offset, rules, points, type, props = [], children = [], length), rulesets);
					if (character === 123) if (offset === 0) parse(characters, root, reference, reference, props, rulesets, length, points, children);
					else switch (atrule === 99 && charat(characters, 3) === 110 ? 100 : atrule) {
						case 100:
						case 108:
						case 109:
						case 115:
							parse(value, reference, reference, rule && append(ruleset(value, reference, reference, 0, 0, rules, points, type, rules, props = [], length), children), rules, children, length, points, rule ? props : children);
							break;
						default: parse(characters, reference, reference, reference, [""], children, 0, points, children);
					}
			}
			index = offset = property = 0, variable = ampersand = 1, type = characters = "", length = pseudo;
			break;
		case 58: length = 1 + strlen(characters), property = previous;
		default:
			if (variable < 1) {
				if (character == 123) --variable;
				else if (character == 125 && variable++ == 0 && prev() == 125) continue;
			}
			switch (characters += from(character), character * variable) {
				case 38:
					ampersand = offset > 0 ? 1 : (characters += "\f", -1);
					break;
				case 44:
					points[index++] = (strlen(characters) - 1) * ampersand, ampersand = 1;
					break;
				case 64:
					if (peek() === 45) characters += delimit(next());
					atrule = peek(), offset = length = strlen(type = characters += identifier(caret())), character++;
					break;
				case 45: if (previous === 45 && strlen(characters) == 2) variable = 0;
			}
	}
	return rulesets;
}
/**
* @param {string} value
* @param {object} root
* @param {object?} parent
* @param {number} index
* @param {number} offset
* @param {string[]} rules
* @param {number[]} points
* @param {string} type
* @param {string[]} props
* @param {string[]} children
* @param {number} length
* @return {object}
*/
function ruleset(value, root, parent, index, offset, rules, points, type, props, children, length) {
	var post = offset - 1;
	var rule = offset === 0 ? rules : [""];
	var size = sizeof(rule);
	for (var i = 0, j = 0, k = 0; i < index; ++i) for (var x = 0, y = substr(value, post + 1, post = abs(j = points[i])), z = value; x < size; ++x) if (z = trim(j > 0 ? rule[x] + " " + y : replace(y, /&\f/g, rule[x]))) props[k++] = z;
	return node(value, root, parent, offset === 0 ? RULESET : type, props, children, length);
}
/**
* @param {number} value
* @param {object} root
* @param {object?} parent
* @return {object}
*/
function comment(value, root, parent) {
	return node(value, root, parent, COMMENT, from(char()), substr(value, 2, -2), 0);
}
/**
* @param {string} value
* @param {object} root
* @param {object?} parent
* @param {number} length
* @return {object}
*/
function declaration(value, root, parent, length) {
	return node(value, root, parent, DECLARATION, substr(value, 0, length), substr(value, length + 1, -1), length);
}
//#endregion
//#region node_modules/stylis/src/Serializer.js
/**
* @param {object[]} children
* @param {function} callback
* @return {string}
*/
function serialize(children, callback) {
	var output = "";
	var length = sizeof(children);
	for (var i = 0; i < length; i++) output += callback(children[i], i, children, callback) || "";
	return output;
}
/**
* @param {object} element
* @param {number} index
* @param {object[]} children
* @param {function} callback
* @return {string}
*/
function stringify(element, index, children, callback) {
	switch (element.type) {
		case LAYER: if (element.children.length) break;
		case IMPORT:
		case DECLARATION: return element.return = element.return || element.value;
		case COMMENT: return "";
		case KEYFRAMES: return element.return = element.value + "{" + serialize(element.children, callback) + "}";
		case RULESET: element.value = element.props.join(",");
	}
	return strlen(children = serialize(element.children, callback)) ? element.return = element.value + "{" + children + "}" : "";
}
//#endregion
//#region node_modules/stylis/src/Middleware.js
/**
* @param {function[]} collection
* @return {function}
*/
function middleware(collection) {
	var length = sizeof(collection);
	return function(element, index, children, callback) {
		var output = "";
		for (var i = 0; i < length; i++) output += collection[i](element, index, children, callback) || "";
		return output;
	};
}
/**
* @param {function} callback
* @return {function}
*/
function rulesheet(callback) {
	return function(element) {
		if (!element.root) {
			if (element = element.return) callback(element);
		}
	};
}
//#endregion
//#region node_modules/@emotion/memoize/dist/emotion-memoize.esm.js
function memoize(fn) {
	var cache = Object.create(null);
	return function(arg) {
		if (cache[arg] === void 0) cache[arg] = fn(arg);
		return cache[arg];
	};
}
//#endregion
//#region node_modules/@emotion/cache/dist/emotion-cache.browser.esm.js
var identifierWithPointTracking = function identifierWithPointTracking(begin, points, index) {
	var previous = 0;
	var character = 0;
	while (true) {
		previous = character;
		character = peek();
		if (previous === 38 && character === 12) points[index] = 1;
		if (token(character)) break;
		next();
	}
	return slice(begin, position);
};
var toRules = function toRules(parsed, points) {
	var index = -1;
	var character = 44;
	do
		switch (token(character)) {
			case 0:
				if (character === 38 && peek() === 12) points[index] = 1;
				parsed[index] += identifierWithPointTracking(position - 1, points, index);
				break;
			case 2:
				parsed[index] += delimit(character);
				break;
			case 4: if (character === 44) {
				parsed[++index] = peek() === 58 ? "&\f" : "";
				points[index] = parsed[index].length;
				break;
			}
			default: parsed[index] += from(character);
		}
	while (character = next());
	return parsed;
};
var getRules = function getRules(value, points) {
	return dealloc(toRules(alloc(value), points));
};
var fixedElements = /* #__PURE__ */ new WeakMap();
var compat = function compat(element) {
	if (element.type !== "rule" || !element.parent || element.length < 1) return;
	var value = element.value;
	var parent = element.parent;
	var isImplicitRule = element.column === parent.column && element.line === parent.line;
	while (parent.type !== "rule") {
		parent = parent.parent;
		if (!parent) return;
	}
	if (element.props.length === 1 && value.charCodeAt(0) !== 58 && !fixedElements.get(parent)) return;
	if (isImplicitRule) return;
	fixedElements.set(element, true);
	var points = [];
	var rules = getRules(value, points);
	var parentRules = parent.props;
	for (var i = 0, k = 0; i < rules.length; i++) for (var j = 0; j < parentRules.length; j++, k++) element.props[k] = points[i] ? rules[i].replace(/&\f/g, parentRules[j]) : parentRules[j] + " " + rules[i];
};
var removeLabel = function removeLabel(element) {
	if (element.type === "decl") {
		var value = element.value;
		if (value.charCodeAt(0) === 108 && value.charCodeAt(2) === 98) {
			element["return"] = "";
			element.value = "";
		}
	}
};
function prefix(value, length) {
	switch (hash(value, length)) {
		case 5103: return WEBKIT + "print-" + value + value;
		case 5737:
		case 4201:
		case 3177:
		case 3433:
		case 1641:
		case 4457:
		case 2921:
		case 5572:
		case 6356:
		case 5844:
		case 3191:
		case 6645:
		case 3005:
		case 6391:
		case 5879:
		case 5623:
		case 6135:
		case 4599:
		case 4855:
		case 4215:
		case 6389:
		case 5109:
		case 5365:
		case 5621:
		case 3829: return WEBKIT + value + value;
		case 5349:
		case 4246:
		case 4810:
		case 6968:
		case 2756: return WEBKIT + value + MOZ + value + MS + value + value;
		case 6828:
		case 4268: return WEBKIT + value + MS + value + value;
		case 6165: return WEBKIT + value + MS + "flex-" + value + value;
		case 5187: return WEBKIT + value + replace(value, /(\w+).+(:[^]+)/, WEBKIT + "box-$1$2" + MS + "flex-$1$2") + value;
		case 5443: return WEBKIT + value + MS + "flex-item-" + replace(value, /flex-|-self/, "") + value;
		case 4675: return WEBKIT + value + MS + "flex-line-pack" + replace(value, /align-content|flex-|-self/, "") + value;
		case 5548: return WEBKIT + value + MS + replace(value, "shrink", "negative") + value;
		case 5292: return WEBKIT + value + MS + replace(value, "basis", "preferred-size") + value;
		case 6060: return WEBKIT + "box-" + replace(value, "-grow", "") + WEBKIT + value + MS + replace(value, "grow", "positive") + value;
		case 4554: return WEBKIT + replace(value, /([^-])(transform)/g, "$1" + WEBKIT + "$2") + value;
		case 6187: return replace(replace(replace(value, /(zoom-|grab)/, WEBKIT + "$1"), /(image-set)/, WEBKIT + "$1"), value, "") + value;
		case 5495:
		case 3959: return replace(value, /(image-set\([^]*)/, WEBKIT + "$1$`$1");
		case 4968: return replace(replace(value, /(.+:)(flex-)?(.*)/, WEBKIT + "box-pack:$3" + MS + "flex-pack:$3"), /s.+-b[^;]+/, "justify") + WEBKIT + value + value;
		case 4095:
		case 3583:
		case 4068:
		case 2532: return replace(value, /(.+)-inline(.+)/, WEBKIT + "$1$2") + value;
		case 8116:
		case 7059:
		case 5753:
		case 5535:
		case 5445:
		case 5701:
		case 4933:
		case 4677:
		case 5533:
		case 5789:
		case 5021:
		case 4765:
			if (strlen(value) - 1 - length > 6) switch (charat(value, length + 1)) {
				case 109: if (charat(value, length + 4) !== 45) break;
				case 102: return replace(value, /(.+:)(.+)-([^]+)/, "$1" + WEBKIT + "$2-$3$1" + MOZ + (charat(value, length + 3) == 108 ? "$3" : "$2-$3")) + value;
				case 115: return ~indexof(value, "stretch") ? prefix(replace(value, "stretch", "fill-available"), length) + value : value;
			}
			break;
		case 4949: if (charat(value, length + 1) !== 115) break;
		case 6444:
			switch (charat(value, strlen(value) - 3 - (~indexof(value, "!important") && 10))) {
				case 107: return replace(value, ":", ":" + WEBKIT) + value;
				case 101: return replace(value, /(.+:)([^;!]+)(;|!.+)?/, "$1" + WEBKIT + (charat(value, 14) === 45 ? "inline-" : "") + "box$3$1" + WEBKIT + "$2$3$1" + MS + "$2box$3") + value;
			}
			break;
		case 5936:
			switch (charat(value, length + 11)) {
				case 114: return WEBKIT + value + MS + replace(value, /[svh]\w+-[tblr]{2}/, "tb") + value;
				case 108: return WEBKIT + value + MS + replace(value, /[svh]\w+-[tblr]{2}/, "tb-rl") + value;
				case 45: return WEBKIT + value + MS + replace(value, /[svh]\w+-[tblr]{2}/, "lr") + value;
			}
			return WEBKIT + value + MS + value + value;
	}
	return value;
}
var defaultStylisPlugins = [function prefixer(element, index, children, callback) {
	if (element.length > -1) {
		if (!element["return"]) switch (element.type) {
			case DECLARATION:
				element["return"] = prefix(element.value, element.length);
				break;
			case KEYFRAMES: return serialize([copy(element, { value: replace(element.value, "@", "@" + WEBKIT) })], callback);
			case RULESET: if (element.length) return combine(element.props, function(value) {
				switch (match(value, /(::plac\w+|:read-\w+)/)) {
					case ":read-only":
					case ":read-write": return serialize([copy(element, { props: [replace(value, /:(read-\w+)/, ":" + MOZ + "$1")] })], callback);
					case "::placeholder": return serialize([
						copy(element, { props: [replace(value, /:(plac\w+)/, ":" + WEBKIT + "input-$1")] }),
						copy(element, { props: [replace(value, /:(plac\w+)/, ":" + MOZ + "$1")] }),
						copy(element, { props: [replace(value, /:(plac\w+)/, MS + "input-$1")] })
					], callback);
				}
				return "";
			});
		}
	}
}];
var createCache = function createCache(options) {
	var key = options.key;
	if (key === "css") {
		var ssrStyles = document.querySelectorAll("style[data-emotion]:not([data-s])");
		Array.prototype.forEach.call(ssrStyles, function(node) {
			if (node.getAttribute("data-emotion").indexOf(" ") === -1) return;
			document.head.appendChild(node);
			node.setAttribute("data-s", "");
		});
	}
	var stylisPlugins = options.stylisPlugins || defaultStylisPlugins;
	var inserted = {};
	var container;
	var nodesToHydrate = [];
	container = options.container || document.head;
	Array.prototype.forEach.call(document.querySelectorAll("style[data-emotion^=\"" + key + " \"]"), function(node) {
		var attrib = node.getAttribute("data-emotion").split(" ");
		for (var i = 1; i < attrib.length; i++) inserted[attrib[i]] = true;
		nodesToHydrate.push(node);
	});
	var _insert;
	var omnipresentPlugins = [compat, removeLabel];
	var currentSheet;
	var finalizingPlugins = [stringify, rulesheet(function(rule) {
		currentSheet.insert(rule);
	})];
	var serializer = middleware(omnipresentPlugins.concat(stylisPlugins, finalizingPlugins));
	var stylis = function stylis(styles) {
		return serialize(compile(styles), serializer);
	};
	_insert = function insert(selector, serialized, sheet, shouldCache) {
		currentSheet = sheet;
		stylis(selector ? selector + "{" + serialized.styles + "}" : serialized.styles);
		if (shouldCache) cache.inserted[serialized.name] = true;
	};
	var cache = {
		key,
		sheet: new StyleSheet({
			key,
			container,
			nonce: options.nonce,
			speedy: options.speedy,
			prepend: options.prepend,
			insertionPoint: options.insertionPoint
		}),
		nonce: options.nonce,
		inserted,
		registered: {},
		insert: _insert
	};
	cache.sheet.hydrate(nodesToHydrate);
	return cache;
};
//#endregion
//#region node_modules/react-is/cjs/react-is.production.min.js
/** @license React v16.13.1
* react-is.production.min.js
*
* Copyright (c) Facebook, Inc. and its affiliates.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/
var require_react_is_production_min = /* @__PURE__ */ __commonJSMin(((exports) => {
	var b = "function" === typeof Symbol && Symbol.for, c = b ? Symbol.for("react.element") : 60103, d = b ? Symbol.for("react.portal") : 60106, e = b ? Symbol.for("react.fragment") : 60107, f = b ? Symbol.for("react.strict_mode") : 60108, g = b ? Symbol.for("react.profiler") : 60114, h = b ? Symbol.for("react.provider") : 60109, k = b ? Symbol.for("react.context") : 60110, l = b ? Symbol.for("react.async_mode") : 60111, m = b ? Symbol.for("react.concurrent_mode") : 60111, n = b ? Symbol.for("react.forward_ref") : 60112, p = b ? Symbol.for("react.suspense") : 60113, q = b ? Symbol.for("react.suspense_list") : 60120, r = b ? Symbol.for("react.memo") : 60115, t = b ? Symbol.for("react.lazy") : 60116, v = b ? Symbol.for("react.block") : 60121, w = b ? Symbol.for("react.fundamental") : 60117, x = b ? Symbol.for("react.responder") : 60118, y = b ? Symbol.for("react.scope") : 60119;
	function z(a) {
		if ("object" === typeof a && null !== a) {
			var u = a.$$typeof;
			switch (u) {
				case c: switch (a = a.type, a) {
					case l:
					case m:
					case e:
					case g:
					case f:
					case p: return a;
					default: switch (a = a && a.$$typeof, a) {
						case k:
						case n:
						case t:
						case r:
						case h: return a;
						default: return u;
					}
				}
				case d: return u;
			}
		}
	}
	function A(a) {
		return z(a) === m;
	}
	exports.AsyncMode = l;
	exports.ConcurrentMode = m;
	exports.ContextConsumer = k;
	exports.ContextProvider = h;
	exports.Element = c;
	exports.ForwardRef = n;
	exports.Fragment = e;
	exports.Lazy = t;
	exports.Memo = r;
	exports.Portal = d;
	exports.Profiler = g;
	exports.StrictMode = f;
	exports.Suspense = p;
	exports.isAsyncMode = function(a) {
		return A(a) || z(a) === l;
	};
	exports.isConcurrentMode = A;
	exports.isContextConsumer = function(a) {
		return z(a) === k;
	};
	exports.isContextProvider = function(a) {
		return z(a) === h;
	};
	exports.isElement = function(a) {
		return "object" === typeof a && null !== a && a.$$typeof === c;
	};
	exports.isForwardRef = function(a) {
		return z(a) === n;
	};
	exports.isFragment = function(a) {
		return z(a) === e;
	};
	exports.isLazy = function(a) {
		return z(a) === t;
	};
	exports.isMemo = function(a) {
		return z(a) === r;
	};
	exports.isPortal = function(a) {
		return z(a) === d;
	};
	exports.isProfiler = function(a) {
		return z(a) === g;
	};
	exports.isStrictMode = function(a) {
		return z(a) === f;
	};
	exports.isSuspense = function(a) {
		return z(a) === p;
	};
	exports.isValidElementType = function(a) {
		return "string" === typeof a || "function" === typeof a || a === e || a === m || a === g || a === f || a === p || a === q || "object" === typeof a && null !== a && (a.$$typeof === t || a.$$typeof === r || a.$$typeof === h || a.$$typeof === k || a.$$typeof === n || a.$$typeof === w || a.$$typeof === x || a.$$typeof === y || a.$$typeof === v);
	};
	exports.typeOf = z;
}));
//#endregion
//#region node_modules/react-is/index.js
var require_react_is = /* @__PURE__ */ __commonJSMin(((exports, module) => {
	module.exports = require_react_is_production_min();
}));
//#endregion
//#region node_modules/hoist-non-react-statics/dist/hoist-non-react-statics.cjs.js
var require_hoist_non_react_statics_cjs = /* @__PURE__ */ __commonJSMin(((exports, module) => {
	var reactIs = require_react_is();
	/**
	* Copyright 2015, Yahoo! Inc.
	* Copyrights licensed under the New BSD License. See the accompanying LICENSE file for terms.
	*/
	var REACT_STATICS = {
		childContextTypes: true,
		contextType: true,
		contextTypes: true,
		defaultProps: true,
		displayName: true,
		getDefaultProps: true,
		getDerivedStateFromError: true,
		getDerivedStateFromProps: true,
		mixins: true,
		propTypes: true,
		type: true
	};
	var KNOWN_STATICS = {
		name: true,
		length: true,
		prototype: true,
		caller: true,
		callee: true,
		arguments: true,
		arity: true
	};
	var FORWARD_REF_STATICS = {
		"$$typeof": true,
		render: true,
		defaultProps: true,
		displayName: true,
		propTypes: true
	};
	var MEMO_STATICS = {
		"$$typeof": true,
		compare: true,
		defaultProps: true,
		displayName: true,
		propTypes: true,
		type: true
	};
	var TYPE_STATICS = {};
	TYPE_STATICS[reactIs.ForwardRef] = FORWARD_REF_STATICS;
	TYPE_STATICS[reactIs.Memo] = MEMO_STATICS;
	function getStatics(component) {
		if (reactIs.isMemo(component)) return MEMO_STATICS;
		return TYPE_STATICS[component["$$typeof"]] || REACT_STATICS;
	}
	var defineProperty = Object.defineProperty;
	var getOwnPropertyNames = Object.getOwnPropertyNames;
	var getOwnPropertySymbols = Object.getOwnPropertySymbols;
	var getOwnPropertyDescriptor = Object.getOwnPropertyDescriptor;
	var getPrototypeOf = Object.getPrototypeOf;
	var objectPrototype = Object.prototype;
	function hoistNonReactStatics(targetComponent, sourceComponent, blacklist) {
		if (typeof sourceComponent !== "string") {
			if (objectPrototype) {
				var inheritedComponent = getPrototypeOf(sourceComponent);
				if (inheritedComponent && inheritedComponent !== objectPrototype) hoistNonReactStatics(targetComponent, inheritedComponent, blacklist);
			}
			var keys = getOwnPropertyNames(sourceComponent);
			if (getOwnPropertySymbols) keys = keys.concat(getOwnPropertySymbols(sourceComponent));
			var targetStatics = getStatics(targetComponent);
			var sourceStatics = getStatics(sourceComponent);
			for (var i = 0; i < keys.length; ++i) {
				var key = keys[i];
				if (!KNOWN_STATICS[key] && !(blacklist && blacklist[key]) && !(sourceStatics && sourceStatics[key]) && !(targetStatics && targetStatics[key])) {
					var descriptor = getOwnPropertyDescriptor(sourceComponent, key);
					try {
						defineProperty(targetComponent, key, descriptor);
					} catch (e) {}
				}
			}
		}
		return targetComponent;
	}
	module.exports = hoistNonReactStatics;
}));
//#endregion
//#region node_modules/@emotion/utils/dist/emotion-utils.browser.esm.js
function getRegisteredStyles(registered, registeredStyles, classNames) {
	var rawClassName = "";
	classNames.split(" ").forEach(function(className) {
		if (registered[className] !== void 0) registeredStyles.push(registered[className] + ";");
		else if (className) rawClassName += className + " ";
	});
	return rawClassName;
}
var registerStyles = function registerStyles(cache, serialized, isStringTag) {
	var className = cache.key + "-" + serialized.name;
	if ((isStringTag === false || false) && cache.registered[className] === void 0) cache.registered[className] = serialized.styles;
};
var insertStyles = function insertStyles(cache, serialized, isStringTag) {
	registerStyles(cache, serialized, isStringTag);
	var className = cache.key + "-" + serialized.name;
	if (cache.inserted[serialized.name] === void 0) {
		var current = serialized;
		do {
			cache.insert(serialized === current ? "." + className : "", current, cache.sheet, true);
			current = current.next;
		} while (current !== void 0);
	}
};
//#endregion
//#region node_modules/@emotion/hash/dist/emotion-hash.esm.js
function murmur2(str) {
	var h = 0;
	var k, i = 0, len = str.length;
	for (; len >= 4; ++i, len -= 4) {
		k = str.charCodeAt(i) & 255 | (str.charCodeAt(++i) & 255) << 8 | (str.charCodeAt(++i) & 255) << 16 | (str.charCodeAt(++i) & 255) << 24;
		k = (k & 65535) * 1540483477 + ((k >>> 16) * 59797 << 16);
		k ^= k >>> 24;
		h = (k & 65535) * 1540483477 + ((k >>> 16) * 59797 << 16) ^ (h & 65535) * 1540483477 + ((h >>> 16) * 59797 << 16);
	}
	switch (len) {
		case 3: h ^= (str.charCodeAt(i + 2) & 255) << 16;
		case 2: h ^= (str.charCodeAt(i + 1) & 255) << 8;
		case 1:
			h ^= str.charCodeAt(i) & 255;
			h = (h & 65535) * 1540483477 + ((h >>> 16) * 59797 << 16);
	}
	h ^= h >>> 13;
	h = (h & 65535) * 1540483477 + ((h >>> 16) * 59797 << 16);
	return ((h ^ h >>> 15) >>> 0).toString(36);
}
//#endregion
//#region node_modules/@emotion/unitless/dist/emotion-unitless.esm.js
var unitlessKeys = {
	animationIterationCount: 1,
	aspectRatio: 1,
	borderImageOutset: 1,
	borderImageSlice: 1,
	borderImageWidth: 1,
	boxFlex: 1,
	boxFlexGroup: 1,
	boxOrdinalGroup: 1,
	columnCount: 1,
	columns: 1,
	flex: 1,
	flexGrow: 1,
	flexPositive: 1,
	flexShrink: 1,
	flexNegative: 1,
	flexOrder: 1,
	gridRow: 1,
	gridRowEnd: 1,
	gridRowSpan: 1,
	gridRowStart: 1,
	gridColumn: 1,
	gridColumnEnd: 1,
	gridColumnSpan: 1,
	gridColumnStart: 1,
	msGridRow: 1,
	msGridRowSpan: 1,
	msGridColumn: 1,
	msGridColumnSpan: 1,
	fontWeight: 1,
	lineHeight: 1,
	opacity: 1,
	order: 1,
	orphans: 1,
	scale: 1,
	tabSize: 1,
	widows: 1,
	zIndex: 1,
	zoom: 1,
	WebkitLineClamp: 1,
	fillOpacity: 1,
	floodOpacity: 1,
	stopOpacity: 1,
	strokeDasharray: 1,
	strokeDashoffset: 1,
	strokeMiterlimit: 1,
	strokeOpacity: 1,
	strokeWidth: 1
};
//#endregion
//#region node_modules/@emotion/serialize/dist/emotion-serialize.esm.js
var isDevelopment$1 = false;
var hyphenateRegex = /[A-Z]|^ms/g;
var animationRegex = /_EMO_([^_]+?)_([^]*?)_EMO_/g;
var isCustomProperty = function isCustomProperty(property) {
	return property.charCodeAt(1) === 45;
};
var isProcessableValue = function isProcessableValue(value) {
	return value != null && typeof value !== "boolean";
};
var processStyleName = /* #__PURE__ */ memoize(function(styleName) {
	return isCustomProperty(styleName) ? styleName : styleName.replace(hyphenateRegex, "-$&").toLowerCase();
});
var processStyleValue = function processStyleValue(key, value) {
	switch (key) {
		case "animation":
		case "animationName": if (typeof value === "string") return value.replace(animationRegex, function(match, p1, p2) {
			cursor = {
				name: p1,
				styles: p2,
				next: cursor
			};
			return p1;
		});
	}
	if (unitlessKeys[key] !== 1 && !isCustomProperty(key) && typeof value === "number" && value !== 0) return value + "px";
	return value;
};
var noComponentSelectorMessage = "Component selectors can only be used in conjunction with @emotion/babel-plugin, the swc Emotion plugin, or another Emotion-aware compiler transform.";
function handleInterpolation(mergedProps, registered, interpolation) {
	if (interpolation == null) return "";
	var componentSelector = interpolation;
	if (componentSelector.__emotion_styles !== void 0) return componentSelector;
	switch (typeof interpolation) {
		case "boolean": return "";
		case "object":
			var keyframes = interpolation;
			if (keyframes.anim === 1) {
				cursor = {
					name: keyframes.name,
					styles: keyframes.styles,
					next: cursor
				};
				return keyframes.name;
			}
			var serializedStyles = interpolation;
			if (serializedStyles.styles !== void 0) {
				var next = serializedStyles.next;
				if (next !== void 0) while (next !== void 0) {
					cursor = {
						name: next.name,
						styles: next.styles,
						next: cursor
					};
					next = next.next;
				}
				return serializedStyles.styles + ";";
			}
			return createStringFromObject(mergedProps, registered, interpolation);
		case "function":
			if (mergedProps !== void 0) {
				var previousCursor = cursor;
				var result = interpolation(mergedProps);
				cursor = previousCursor;
				return handleInterpolation(mergedProps, registered, result);
			}
			break;
	}
	var asString = interpolation;
	if (registered == null) return asString;
	var cached = registered[asString];
	return cached !== void 0 ? cached : asString;
}
function createStringFromObject(mergedProps, registered, obj) {
	var string = "";
	if (Array.isArray(obj)) for (var i = 0; i < obj.length; i++) string += handleInterpolation(mergedProps, registered, obj[i]) + ";";
	else for (var key in obj) {
		var value = obj[key];
		if (typeof value !== "object") {
			var asString = value;
			if (registered != null && registered[asString] !== void 0) string += key + "{" + registered[asString] + "}";
			else if (isProcessableValue(asString)) string += processStyleName(key) + ":" + processStyleValue(key, asString) + ";";
		} else {
			if (key === "NO_COMPONENT_SELECTOR" && isDevelopment$1) throw new Error(noComponentSelectorMessage);
			if (Array.isArray(value) && typeof value[0] === "string" && (registered == null || registered[value[0]] === void 0)) {
				for (var _i = 0; _i < value.length; _i++) if (isProcessableValue(value[_i])) string += processStyleName(key) + ":" + processStyleValue(key, value[_i]) + ";";
			} else {
				var interpolated = handleInterpolation(mergedProps, registered, value);
				switch (key) {
					case "animation":
					case "animationName":
						string += processStyleName(key) + ":" + interpolated + ";";
						break;
					default: string += key + "{" + interpolated + "}";
				}
			}
		}
	}
	return string;
}
var labelPattern = /label:\s*([^\s;{]+)\s*(;|$)/g;
var cursor;
function serializeStyles(args, registered, mergedProps) {
	if (args.length === 1 && typeof args[0] === "object" && args[0] !== null && args[0].styles !== void 0) return args[0];
	var stringMode = true;
	var styles = "";
	cursor = void 0;
	var strings = args[0];
	if (strings == null || strings.raw === void 0) {
		stringMode = false;
		styles += handleInterpolation(mergedProps, registered, strings);
	} else styles += strings[0];
	for (var i = 1; i < args.length; i++) {
		styles += handleInterpolation(mergedProps, registered, args[i]);
		if (stringMode) styles += strings[i];
	}
	labelPattern.lastIndex = 0;
	var identifierName = "";
	var match;
	while ((match = labelPattern.exec(styles)) !== null) identifierName += "-" + match[1];
	return {
		name: murmur2(styles) + identifierName,
		styles,
		next: cursor
	};
}
//#endregion
//#region node_modules/@emotion/use-insertion-effect-with-fallbacks/dist/emotion-use-insertion-effect-with-fallbacks.browser.esm.js
var syncFallback = function syncFallback(create) {
	return create();
};
var useInsertionEffect = import_react.useInsertionEffect ? import_react.useInsertionEffect : false;
var useInsertionEffectAlwaysWithSyncFallback = useInsertionEffect || syncFallback;
useInsertionEffect || import_react.useLayoutEffect;
var EmotionCacheContext = /* #__PURE__ */ import_react.createContext(typeof HTMLElement !== "undefined" ? /* #__PURE__ */ createCache({ key: "css" }) : null);
EmotionCacheContext.Provider;
var withEmotionCache = function withEmotionCache(func) {
	return /*#__PURE__*/ (0, import_react.forwardRef)(function(props, ref) {
		return func(props, (0, import_react.useContext)(EmotionCacheContext), ref);
	});
};
var ThemeContext = /* #__PURE__ */ import_react.createContext({});
var hasOwn = {}.hasOwnProperty;
var typePropName = "__EMOTION_TYPE_PLEASE_DO_NOT_USE__";
var createEmotionProps = function createEmotionProps(type, props) {
	var newProps = {};
	for (var _key in props) if (hasOwn.call(props, _key)) newProps[_key] = props[_key];
	newProps[typePropName] = type;
	return newProps;
};
var Insertion = function Insertion(_ref) {
	var cache = _ref.cache, serialized = _ref.serialized, isStringTag = _ref.isStringTag;
	registerStyles(cache, serialized, isStringTag);
	useInsertionEffectAlwaysWithSyncFallback(function() {
		return insertStyles(cache, serialized, isStringTag);
	});
	return null;
};
var Emotion$1 = /* @__PURE__ */ withEmotionCache(function(props, cache, ref) {
	var cssProp = props.css;
	if (typeof cssProp === "string" && cache.registered[cssProp] !== void 0) cssProp = cache.registered[cssProp];
	var WrappedComponent = props[typePropName];
	var registeredStyles = [cssProp];
	var className = "";
	if (typeof props.className === "string") className = getRegisteredStyles(cache.registered, registeredStyles, props.className);
	else if (props.className != null) className = props.className + " ";
	var serialized = serializeStyles(registeredStyles, void 0, import_react.useContext(ThemeContext));
	className += cache.key + "-" + serialized.name;
	var newProps = {};
	for (var _key2 in props) if (hasOwn.call(props, _key2) && _key2 !== "css" && _key2 !== typePropName && true) newProps[_key2] = props[_key2];
	newProps.className = className;
	if (ref) newProps.ref = ref;
	return /*#__PURE__*/ import_react.createElement(import_react.Fragment, null, /*#__PURE__*/ import_react.createElement(Insertion, {
		cache,
		serialized,
		isStringTag: typeof WrappedComponent === "string"
	}), /*#__PURE__*/ import_react.createElement(WrappedComponent, newProps));
});
require_hoist_non_react_statics_cjs();
var jsx = function jsx(type, props) {
	var args = arguments;
	if (props == null || !hasOwn.call(props, "css")) return import_react.createElement.apply(void 0, args);
	var argsLength = args.length;
	var createElementArgArray = new Array(argsLength);
	createElementArgArray[0] = Emotion$1;
	createElementArgArray[1] = createEmotionProps(type, props);
	for (var i = 2; i < argsLength; i++) createElementArgArray[i] = args[i];
	return import_react.createElement.apply(null, createElementArgArray);
};
(function(_jsx) {
	var JSX;
	JSX || (JSX = _jsx.JSX || (_jsx.JSX = {}));
})(jsx || (jsx = {}));
function css$2() {
	for (var _len = arguments.length, args = new Array(_len), _key = 0; _key < _len; _key++) args[_key] = arguments[_key];
	return serializeStyles(args);
}
function keyframes() {
	var insertable = css$2.apply(void 0, arguments);
	var name = "animation-" + insertable.name;
	return {
		name,
		styles: "@keyframes " + name + "{" + insertable.styles + "}",
		anim: 1,
		toString: function toString() {
			return "_EMO_" + this.name + "_" + this.styles + "_EMO_";
		}
	};
}
//#endregion
//#region node_modules/@babel/runtime/helpers/esm/taggedTemplateLiteral.js
function _taggedTemplateLiteral(e, t) {
	return t || (t = e.slice(0)), Object.freeze(Object.defineProperties(e, { raw: { value: Object.freeze(t) } }));
}
//#endregion
//#region node_modules/@floating-ui/utils/dist/floating-ui.utils.mjs
var import_react_dom = require_react_dom();
var min = Math.min;
var max = Math.max;
var round = Math.round;
var floor = Math.floor;
var createCoords = (v) => ({
	x: v,
	y: v
});
function rectToClientRect(rect) {
	const { x, y, width, height } = rect;
	return {
		width,
		height,
		top: y,
		left: x,
		right: x + width,
		bottom: y + height,
		x,
		y
	};
}
//#endregion
//#region node_modules/@floating-ui/utils/dist/floating-ui.utils.dom.mjs
function hasWindow() {
	return typeof window !== "undefined";
}
function getNodeName(node) {
	if (isNode(node)) return (node.nodeName || "").toLowerCase();
	return "#document";
}
function getWindow(node) {
	var _node$ownerDocument;
	return (node == null || (_node$ownerDocument = node.ownerDocument) == null ? void 0 : _node$ownerDocument.defaultView) || window;
}
function getDocumentElement(node) {
	var _ref;
	return (_ref = (isNode(node) ? node.ownerDocument : node.document) || window.document) == null ? void 0 : _ref.documentElement;
}
function isNode(value) {
	if (!hasWindow()) return false;
	return value instanceof Node || value instanceof getWindow(value).Node;
}
function isElement(value) {
	if (!hasWindow()) return false;
	return value instanceof Element || value instanceof getWindow(value).Element;
}
function isHTMLElement(value) {
	if (!hasWindow()) return false;
	return value instanceof HTMLElement || value instanceof getWindow(value).HTMLElement;
}
function isShadowRoot(value) {
	if (!hasWindow() || typeof ShadowRoot === "undefined") return false;
	return value instanceof ShadowRoot || value instanceof getWindow(value).ShadowRoot;
}
function isOverflowElement(element) {
	const { overflow, overflowX, overflowY, display } = getComputedStyle$1(element);
	return /auto|scroll|overlay|hidden|clip/.test(overflow + overflowY + overflowX) && display !== "inline" && display !== "contents";
}
var isWebKitValue;
function isWebKit() {
	if (isWebKitValue == null) isWebKitValue = typeof CSS !== "undefined" && CSS.supports && CSS.supports("-webkit-backdrop-filter", "none");
	return isWebKitValue;
}
function isLastTraversableNode(node) {
	return /^(html|body|#document)$/.test(getNodeName(node));
}
function getComputedStyle$1(element) {
	return getWindow(element).getComputedStyle(element);
}
function getParentNode(node) {
	if (getNodeName(node) === "html") return node;
	const result = node.assignedSlot || node.parentNode || isShadowRoot(node) && node.host || getDocumentElement(node);
	return isShadowRoot(result) ? result.host : result;
}
function getNearestOverflowAncestor(node) {
	const parentNode = getParentNode(node);
	if (isLastTraversableNode(parentNode)) return node.ownerDocument ? node.ownerDocument.body : node.body;
	if (isHTMLElement(parentNode) && isOverflowElement(parentNode)) return parentNode;
	return getNearestOverflowAncestor(parentNode);
}
function getOverflowAncestors(node, list, traverseIframes) {
	var _node$ownerDocument2;
	if (list === void 0) list = [];
	if (traverseIframes === void 0) traverseIframes = true;
	const scrollableAncestor = getNearestOverflowAncestor(node);
	const isBody = scrollableAncestor === ((_node$ownerDocument2 = node.ownerDocument) == null ? void 0 : _node$ownerDocument2.body);
	const win = getWindow(scrollableAncestor);
	if (isBody) {
		const frameElement = getFrameElement(win);
		return list.concat(win, win.visualViewport || [], isOverflowElement(scrollableAncestor) ? scrollableAncestor : [], frameElement && traverseIframes ? getOverflowAncestors(frameElement) : []);
	} else return list.concat(scrollableAncestor, getOverflowAncestors(scrollableAncestor, [], traverseIframes));
}
function getFrameElement(win) {
	return win.parent && Object.getPrototypeOf(win.parent) ? win.frameElement : null;
}
//#endregion
//#region node_modules/@floating-ui/dom/dist/floating-ui.dom.mjs
function getCssDimensions(element) {
	const css = getComputedStyle$1(element);
	let width = parseFloat(css.width) || 0;
	let height = parseFloat(css.height) || 0;
	const hasOffset = isHTMLElement(element);
	const offsetWidth = hasOffset ? element.offsetWidth : width;
	const offsetHeight = hasOffset ? element.offsetHeight : height;
	const shouldFallback = round(width) !== offsetWidth || round(height) !== offsetHeight;
	if (shouldFallback) {
		width = offsetWidth;
		height = offsetHeight;
	}
	return {
		width,
		height,
		$: shouldFallback
	};
}
function unwrapElement(element) {
	return !isElement(element) ? element.contextElement : element;
}
function getScale(element) {
	const domElement = unwrapElement(element);
	if (!isHTMLElement(domElement)) return createCoords(1);
	const rect = domElement.getBoundingClientRect();
	const { width, height, $ } = getCssDimensions(domElement);
	let x = ($ ? round(rect.width) : rect.width) / width;
	let y = ($ ? round(rect.height) : rect.height) / height;
	if (!x || !Number.isFinite(x)) x = 1;
	if (!y || !Number.isFinite(y)) y = 1;
	return {
		x,
		y
	};
}
var noOffsets = /*#__PURE__*/ createCoords(0);
function getVisualOffsets(element) {
	const win = getWindow(element);
	if (!isWebKit() || !win.visualViewport) return noOffsets;
	return {
		x: win.visualViewport.offsetLeft,
		y: win.visualViewport.offsetTop
	};
}
function shouldAddVisualOffsets(element, isFixed, floatingOffsetParent) {
	if (isFixed === void 0) isFixed = false;
	if (!floatingOffsetParent || isFixed && floatingOffsetParent !== getWindow(element)) return false;
	return isFixed;
}
function getBoundingClientRect(element, includeScale, isFixedStrategy, offsetParent) {
	if (includeScale === void 0) includeScale = false;
	if (isFixedStrategy === void 0) isFixedStrategy = false;
	const clientRect = element.getBoundingClientRect();
	const domElement = unwrapElement(element);
	let scale = createCoords(1);
	if (includeScale) if (offsetParent) {
		if (isElement(offsetParent)) scale = getScale(offsetParent);
	} else scale = getScale(element);
	const visualOffsets = shouldAddVisualOffsets(domElement, isFixedStrategy, offsetParent) ? getVisualOffsets(domElement) : createCoords(0);
	let x = (clientRect.left + visualOffsets.x) / scale.x;
	let y = (clientRect.top + visualOffsets.y) / scale.y;
	let width = clientRect.width / scale.x;
	let height = clientRect.height / scale.y;
	if (domElement) {
		const win = getWindow(domElement);
		const offsetWin = offsetParent && isElement(offsetParent) ? getWindow(offsetParent) : offsetParent;
		let currentWin = win;
		let currentIFrame = getFrameElement(currentWin);
		while (currentIFrame && offsetParent && offsetWin !== currentWin) {
			const iframeScale = getScale(currentIFrame);
			const iframeRect = currentIFrame.getBoundingClientRect();
			const css = getComputedStyle$1(currentIFrame);
			const left = iframeRect.left + (currentIFrame.clientLeft + parseFloat(css.paddingLeft)) * iframeScale.x;
			const top = iframeRect.top + (currentIFrame.clientTop + parseFloat(css.paddingTop)) * iframeScale.y;
			x *= iframeScale.x;
			y *= iframeScale.y;
			width *= iframeScale.x;
			height *= iframeScale.y;
			x += left;
			y += top;
			currentWin = getWindow(currentIFrame);
			currentIFrame = getFrameElement(currentWin);
		}
	}
	return rectToClientRect({
		width,
		height,
		x,
		y
	});
}
function rectsAreEqual(a, b) {
	return a.x === b.x && a.y === b.y && a.width === b.width && a.height === b.height;
}
function observeMove(element, onMove) {
	let io = null;
	let timeoutId;
	const root = getDocumentElement(element);
	function cleanup() {
		var _io;
		clearTimeout(timeoutId);
		(_io = io) == null || _io.disconnect();
		io = null;
	}
	function refresh(skip, threshold) {
		if (skip === void 0) skip = false;
		if (threshold === void 0) threshold = 1;
		cleanup();
		const elementRectForRootMargin = element.getBoundingClientRect();
		const { left, top, width, height } = elementRectForRootMargin;
		if (!skip) onMove();
		if (!width || !height) return;
		const insetTop = floor(top);
		const insetRight = floor(root.clientWidth - (left + width));
		const insetBottom = floor(root.clientHeight - (top + height));
		const insetLeft = floor(left);
		const options = {
			rootMargin: -insetTop + "px " + -insetRight + "px " + -insetBottom + "px " + -insetLeft + "px",
			threshold: max(0, min(1, threshold)) || 1
		};
		let isFirstUpdate = true;
		function handleObserve(entries) {
			const ratio = entries[0].intersectionRatio;
			if (ratio !== threshold) {
				if (!isFirstUpdate) return refresh();
				if (!ratio) timeoutId = setTimeout(() => {
					refresh(false, 1e-7);
				}, 1e3);
				else refresh(false, ratio);
			}
			if (ratio === 1 && !rectsAreEqual(elementRectForRootMargin, element.getBoundingClientRect())) refresh();
			isFirstUpdate = false;
		}
		try {
			io = new IntersectionObserver(handleObserve, {
				...options,
				root: root.ownerDocument
			});
		} catch (_e) {
			io = new IntersectionObserver(handleObserve, options);
		}
		io.observe(element);
	}
	refresh(true);
	return cleanup;
}
/**
* Automatically updates the position of the floating element when necessary.
* Should only be called when the floating element is mounted on the DOM or
* visible on the screen.
* @returns cleanup function that should be invoked when the floating element is
* removed from the DOM or hidden from the screen.
* @see https://floating-ui.com/docs/autoUpdate
*/
function autoUpdate(reference, floating, update, options) {
	if (options === void 0) options = {};
	const { ancestorScroll = true, ancestorResize = true, elementResize = typeof ResizeObserver === "function", layoutShift = typeof IntersectionObserver === "function", animationFrame = false } = options;
	const referenceEl = unwrapElement(reference);
	const ancestors = ancestorScroll || ancestorResize ? [...referenceEl ? getOverflowAncestors(referenceEl) : [], ...floating ? getOverflowAncestors(floating) : []] : [];
	ancestors.forEach((ancestor) => {
		ancestorScroll && ancestor.addEventListener("scroll", update, { passive: true });
		ancestorResize && ancestor.addEventListener("resize", update);
	});
	const cleanupIo = referenceEl && layoutShift ? observeMove(referenceEl, update) : null;
	let reobserveFrame = -1;
	let resizeObserver = null;
	if (elementResize) {
		resizeObserver = new ResizeObserver((_ref) => {
			let [firstEntry] = _ref;
			if (firstEntry && firstEntry.target === referenceEl && resizeObserver && floating) {
				resizeObserver.unobserve(floating);
				cancelAnimationFrame(reobserveFrame);
				reobserveFrame = requestAnimationFrame(() => {
					var _resizeObserver;
					(_resizeObserver = resizeObserver) == null || _resizeObserver.observe(floating);
				});
			}
			update();
		});
		if (referenceEl && !animationFrame) resizeObserver.observe(referenceEl);
		if (floating) resizeObserver.observe(floating);
	}
	let frameId;
	let prevRefRect = animationFrame ? getBoundingClientRect(reference) : null;
	if (animationFrame) frameLoop();
	function frameLoop() {
		const nextRefRect = getBoundingClientRect(reference);
		if (prevRefRect && !rectsAreEqual(prevRefRect, nextRefRect)) update();
		prevRefRect = nextRefRect;
		frameId = requestAnimationFrame(frameLoop);
	}
	update();
	return () => {
		var _resizeObserver2;
		ancestors.forEach((ancestor) => {
			ancestorScroll && ancestor.removeEventListener("scroll", update);
			ancestorResize && ancestor.removeEventListener("resize", update);
		});
		cleanupIo?.();
		(_resizeObserver2 = resizeObserver) == null || _resizeObserver2.disconnect();
		resizeObserver = null;
		if (animationFrame) cancelAnimationFrame(frameId);
	};
}
//#endregion
//#region node_modules/use-isomorphic-layout-effect/dist/use-isomorphic-layout-effect.browser.esm.js
var index = import_react.useLayoutEffect;
//#endregion
//#region node_modules/react-select/dist/index-641ee5b8.esm.js
var _excluded$4 = [
	"className",
	"clearValue",
	"cx",
	"getStyles",
	"getClassNames",
	"getValue",
	"hasValue",
	"isMulti",
	"isRtl",
	"options",
	"selectOption",
	"selectProps",
	"setValue",
	"theme"
];
var noop = function noop() {};
/**
String representation of component state for styling with class names.

Expects an array of strings OR a string/object pair:
- className(['comp', 'comp-arg', 'comp-arg-2'])
@returns 'react-select__comp react-select__comp-arg react-select__comp-arg-2'
- className('comp', { some: true, state: false })
@returns 'react-select__comp react-select__comp--some'
*/
function applyPrefixToName(prefix, name) {
	if (!name) return prefix;
	else if (name[0] === "-") return prefix + name;
	else return prefix + "__" + name;
}
function classNames(prefix, state) {
	for (var _len = arguments.length, classNameList = new Array(_len > 2 ? _len - 2 : 0), _key = 2; _key < _len; _key++) classNameList[_key - 2] = arguments[_key];
	var arr = [].concat(classNameList);
	if (state && prefix) {
		for (var key in state) if (state.hasOwnProperty(key) && state[key]) arr.push("".concat(applyPrefixToName(prefix, key)));
	}
	return arr.filter(function(i) {
		return i;
	}).map(function(i) {
		return String(i).trim();
	}).join(" ");
}
var cleanValue = function cleanValue(value) {
	if (isArray(value)) return value.filter(Boolean);
	if (_typeof(value) === "object" && value !== null) return [value];
	return [];
};
var cleanCommonProps = function cleanCommonProps(props) {
	props.className;
	props.clearValue;
	props.cx;
	props.getStyles;
	props.getClassNames;
	props.getValue;
	props.hasValue;
	props.isMulti;
	props.isRtl;
	props.options;
	props.selectOption;
	props.selectProps;
	props.setValue;
	props.theme;
	return _objectSpread2({}, _objectWithoutProperties(props, _excluded$4));
};
var getStyleProps = function getStyleProps(props, name, classNamesState) {
	var cx = props.cx, getStyles = props.getStyles, getClassNames = props.getClassNames, className = props.className;
	return {
		css: getStyles(name, props),
		className: cx(classNamesState !== null && classNamesState !== void 0 ? classNamesState : {}, getClassNames(name, props), className)
	};
};
function isDocumentElement(el) {
	return [
		document.documentElement,
		document.body,
		window
	].indexOf(el) > -1;
}
function normalizedHeight(el) {
	if (isDocumentElement(el)) return window.innerHeight;
	return el.clientHeight;
}
function getScrollTop(el) {
	if (isDocumentElement(el)) return window.pageYOffset;
	return el.scrollTop;
}
function scrollTo(el, top) {
	if (isDocumentElement(el)) {
		window.scrollTo(0, top);
		return;
	}
	el.scrollTop = top;
}
function getScrollParent(element) {
	var style = getComputedStyle(element);
	var excludeStaticParent = style.position === "absolute";
	var overflowRx = /(auto|scroll)/;
	if (style.position === "fixed") return document.documentElement;
	for (var parent = element; parent = parent.parentElement;) {
		style = getComputedStyle(parent);
		if (excludeStaticParent && style.position === "static") continue;
		if (overflowRx.test(style.overflow + style.overflowY + style.overflowX)) return parent;
	}
	return document.documentElement;
}
/**
@param t: time (elapsed)
@param b: initial value
@param c: amount of change
@param d: duration
*/
function easeOutCubic(t, b, c, d) {
	return c * ((t = t / d - 1) * t * t + 1) + b;
}
function animatedScrollTo(element, to) {
	var duration = arguments.length > 2 && arguments[2] !== void 0 ? arguments[2] : 200;
	var callback = arguments.length > 3 && arguments[3] !== void 0 ? arguments[3] : noop;
	var start = getScrollTop(element);
	var change = to - start;
	var increment = 10;
	var currentTime = 0;
	function animateScroll() {
		currentTime += increment;
		scrollTo(element, easeOutCubic(currentTime, start, change, duration));
		if (currentTime < duration) window.requestAnimationFrame(animateScroll);
		else callback(element);
	}
	animateScroll();
}
function scrollIntoView(menuEl, focusedEl) {
	var menuRect = menuEl.getBoundingClientRect();
	var focusedRect = focusedEl.getBoundingClientRect();
	var overScroll = focusedEl.offsetHeight / 3;
	if (focusedRect.bottom + overScroll > menuRect.bottom) scrollTo(menuEl, Math.min(focusedEl.offsetTop + focusedEl.clientHeight - menuEl.offsetHeight + overScroll, menuEl.scrollHeight));
	else if (focusedRect.top - overScroll < menuRect.top) scrollTo(menuEl, Math.max(focusedEl.offsetTop - overScroll, 0));
}
function getBoundingClientObj(element) {
	var rect = element.getBoundingClientRect();
	return {
		bottom: rect.bottom,
		height: rect.height,
		left: rect.left,
		right: rect.right,
		top: rect.top,
		width: rect.width
	};
}
function isTouchCapable() {
	try {
		document.createEvent("TouchEvent");
		return true;
	} catch (e) {
		return false;
	}
}
function isMobileDevice() {
	try {
		return /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent);
	} catch (e) {
		return false;
	}
}
var passiveOptionAccessed = false;
var options = { get passive() {
	return passiveOptionAccessed = true;
} };
var w = typeof window !== "undefined" ? window : {};
if (w.addEventListener && w.removeEventListener) {
	w.addEventListener("p", noop, options);
	w.removeEventListener("p", noop, false);
}
var supportsPassiveEvents = passiveOptionAccessed;
function notNullish(item) {
	return item != null;
}
function isArray(arg) {
	return Array.isArray(arg);
}
function valueTernary(isMulti, multiValue, singleValue) {
	return isMulti ? multiValue : singleValue;
}
function singleValueAsValue(singleValue) {
	return singleValue;
}
function multiValueAsValue(multiValue) {
	return multiValue;
}
var removeProps = function removeProps(propsObj) {
	for (var _len2 = arguments.length, properties = new Array(_len2 > 1 ? _len2 - 1 : 0), _key2 = 1; _key2 < _len2; _key2++) properties[_key2 - 1] = arguments[_key2];
	return Object.entries(propsObj).filter(function(_ref) {
		var key = _slicedToArray(_ref, 1)[0];
		return !properties.includes(key);
	}).reduce(function(newProps, _ref3) {
		var _ref4 = _slicedToArray(_ref3, 2), key = _ref4[0];
		newProps[key] = _ref4[1];
		return newProps;
	}, {});
};
var _excluded$3 = ["children", "innerProps"], _excluded2$1 = ["children", "innerProps"];
function getMenuPlacement(_ref) {
	var preferredMaxHeight = _ref.maxHeight, menuEl = _ref.menuEl, minHeight = _ref.minHeight, preferredPlacement = _ref.placement, shouldScroll = _ref.shouldScroll, isFixedPosition = _ref.isFixedPosition, controlHeight = _ref.controlHeight;
	var scrollParent = getScrollParent(menuEl);
	var defaultState = {
		placement: "bottom",
		maxHeight: preferredMaxHeight
	};
	if (!menuEl || !menuEl.offsetParent) return defaultState;
	var scrollHeight = scrollParent.getBoundingClientRect().height;
	var _menuEl$getBoundingCl = menuEl.getBoundingClientRect(), menuBottom = _menuEl$getBoundingCl.bottom, menuHeight = _menuEl$getBoundingCl.height, menuTop = _menuEl$getBoundingCl.top;
	var containerTop = menuEl.offsetParent.getBoundingClientRect().top;
	var viewHeight = isFixedPosition ? window.innerHeight : normalizedHeight(scrollParent);
	var scrollTop = getScrollTop(scrollParent);
	var marginBottom = parseInt(getComputedStyle(menuEl).marginBottom, 10);
	var marginTop = parseInt(getComputedStyle(menuEl).marginTop, 10);
	var viewSpaceAbove = containerTop - marginTop;
	var viewSpaceBelow = viewHeight - menuTop;
	var scrollSpaceAbove = viewSpaceAbove + scrollTop;
	var scrollSpaceBelow = scrollHeight - scrollTop - menuTop;
	var scrollDown = menuBottom - viewHeight + scrollTop + marginBottom;
	var scrollUp = scrollTop + menuTop - marginTop;
	var scrollDuration = 160;
	switch (preferredPlacement) {
		case "auto":
		case "bottom":
			if (viewSpaceBelow >= menuHeight) return {
				placement: "bottom",
				maxHeight: preferredMaxHeight
			};
			if (scrollSpaceBelow >= menuHeight && !isFixedPosition) {
				if (shouldScroll) animatedScrollTo(scrollParent, scrollDown, scrollDuration);
				return {
					placement: "bottom",
					maxHeight: preferredMaxHeight
				};
			}
			if (!isFixedPosition && scrollSpaceBelow >= minHeight || isFixedPosition && viewSpaceBelow >= minHeight) {
				if (shouldScroll) animatedScrollTo(scrollParent, scrollDown, scrollDuration);
				return {
					placement: "bottom",
					maxHeight: isFixedPosition ? viewSpaceBelow - marginBottom : scrollSpaceBelow - marginBottom
				};
			}
			if (preferredPlacement === "auto" || isFixedPosition) {
				var _constrainedHeight = preferredMaxHeight;
				var spaceAbove = isFixedPosition ? viewSpaceAbove : scrollSpaceAbove;
				if (spaceAbove >= minHeight) _constrainedHeight = Math.min(spaceAbove - marginBottom - controlHeight, preferredMaxHeight);
				return {
					placement: "top",
					maxHeight: _constrainedHeight
				};
			}
			if (preferredPlacement === "bottom") {
				if (shouldScroll) scrollTo(scrollParent, scrollDown);
				return {
					placement: "bottom",
					maxHeight: preferredMaxHeight
				};
			}
			break;
		case "top":
			if (viewSpaceAbove >= menuHeight) return {
				placement: "top",
				maxHeight: preferredMaxHeight
			};
			if (scrollSpaceAbove >= menuHeight && !isFixedPosition) {
				if (shouldScroll) animatedScrollTo(scrollParent, scrollUp, scrollDuration);
				return {
					placement: "top",
					maxHeight: preferredMaxHeight
				};
			}
			if (!isFixedPosition && scrollSpaceAbove >= minHeight || isFixedPosition && viewSpaceAbove >= minHeight) {
				var _constrainedHeight2 = preferredMaxHeight;
				if (!isFixedPosition && scrollSpaceAbove >= minHeight || isFixedPosition && viewSpaceAbove >= minHeight) _constrainedHeight2 = isFixedPosition ? viewSpaceAbove - marginTop : scrollSpaceAbove - marginTop;
				if (shouldScroll) animatedScrollTo(scrollParent, scrollUp, scrollDuration);
				return {
					placement: "top",
					maxHeight: _constrainedHeight2
				};
			}
			return {
				placement: "bottom",
				maxHeight: preferredMaxHeight
			};
		default: throw new Error("Invalid placement provided \"".concat(preferredPlacement, "\"."));
	}
	return defaultState;
}
function alignToControl(placement) {
	return placement ? {
		bottom: "top",
		top: "bottom"
	}[placement] : "bottom";
}
var coercePlacement = function coercePlacement(p) {
	return p === "auto" ? "bottom" : p;
};
var menuCSS = function menuCSS(_ref2, unstyled) {
	var _objectSpread2$1;
	var placement = _ref2.placement, _ref2$theme = _ref2.theme, borderRadius = _ref2$theme.borderRadius, spacing = _ref2$theme.spacing, colors = _ref2$theme.colors;
	return _objectSpread2((_objectSpread2$1 = { label: "menu" }, _defineProperty(_objectSpread2$1, alignToControl(placement), "100%"), _defineProperty(_objectSpread2$1, "position", "absolute"), _defineProperty(_objectSpread2$1, "width", "100%"), _defineProperty(_objectSpread2$1, "zIndex", 1), _objectSpread2$1), unstyled ? {} : {
		backgroundColor: colors.neutral0,
		borderRadius,
		boxShadow: "0 0 0 1px hsla(0, 0%, 0%, 0.1), 0 4px 11px hsla(0, 0%, 0%, 0.1)",
		marginBottom: spacing.menuGutter,
		marginTop: spacing.menuGutter
	});
};
var PortalPlacementContext = /*#__PURE__*/ (0, import_react.createContext)(null);
var MenuPlacer = function MenuPlacer(props) {
	var children = props.children, minMenuHeight = props.minMenuHeight, maxMenuHeight = props.maxMenuHeight, menuPlacement = props.menuPlacement, menuPosition = props.menuPosition, menuShouldScrollIntoView = props.menuShouldScrollIntoView, theme = props.theme;
	var setPortalPlacement = ((0, import_react.useContext)(PortalPlacementContext) || {}).setPortalPlacement;
	var ref = (0, import_react.useRef)(null);
	var _useState2 = _slicedToArray((0, import_react.useState)(maxMenuHeight), 2), maxHeight = _useState2[0], setMaxHeight = _useState2[1];
	var _useState4 = _slicedToArray((0, import_react.useState)(null), 2), placement = _useState4[0], setPlacement = _useState4[1];
	var controlHeight = theme.spacing.controlHeight;
	index(function() {
		var menuEl = ref.current;
		if (!menuEl) return;
		var isFixedPosition = menuPosition === "fixed";
		var state = getMenuPlacement({
			maxHeight: maxMenuHeight,
			menuEl,
			minHeight: minMenuHeight,
			placement: menuPlacement,
			shouldScroll: menuShouldScrollIntoView && !isFixedPosition,
			isFixedPosition,
			controlHeight
		});
		setMaxHeight(state.maxHeight);
		setPlacement(state.placement);
		setPortalPlacement === null || setPortalPlacement === void 0 || setPortalPlacement(state.placement);
	}, [
		maxMenuHeight,
		menuPlacement,
		menuPosition,
		menuShouldScrollIntoView,
		minMenuHeight,
		setPortalPlacement,
		controlHeight
	]);
	return children({
		ref,
		placerProps: _objectSpread2(_objectSpread2({}, props), {}, {
			placement: placement || coercePlacement(menuPlacement),
			maxHeight
		})
	});
};
var Menu$1 = function Menu(props) {
	var children = props.children, innerRef = props.innerRef, innerProps = props.innerProps;
	return jsx("div", _extends({}, getStyleProps(props, "menu", { menu: true }), { ref: innerRef }, innerProps), children);
};
var menuListCSS = function menuListCSS(_ref4, unstyled) {
	var maxHeight = _ref4.maxHeight, baseUnit = _ref4.theme.spacing.baseUnit;
	return _objectSpread2({
		maxHeight,
		overflowY: "auto",
		position: "relative",
		WebkitOverflowScrolling: "touch"
	}, unstyled ? {} : {
		paddingBottom: baseUnit,
		paddingTop: baseUnit
	});
};
var MenuList = function MenuList(props) {
	var children = props.children, innerProps = props.innerProps, innerRef = props.innerRef, isMulti = props.isMulti;
	return jsx("div", _extends({}, getStyleProps(props, "menuList", {
		"menu-list": true,
		"menu-list--is-multi": isMulti
	}), { ref: innerRef }, innerProps), children);
};
var noticeCSS = function noticeCSS(_ref5, unstyled) {
	var _ref5$theme = _ref5.theme, baseUnit = _ref5$theme.spacing.baseUnit, colors = _ref5$theme.colors;
	return _objectSpread2({ textAlign: "center" }, unstyled ? {} : {
		color: colors.neutral40,
		padding: "".concat(baseUnit * 2, "px ").concat(baseUnit * 3, "px")
	});
};
var noOptionsMessageCSS = noticeCSS;
var loadingMessageCSS = noticeCSS;
var NoOptionsMessage = function NoOptionsMessage(_ref6) {
	var _ref6$children = _ref6.children, children = _ref6$children === void 0 ? "No options" : _ref6$children, innerProps = _ref6.innerProps;
	return jsx("div", _extends({}, getStyleProps(_objectSpread2(_objectSpread2({}, _objectWithoutProperties(_ref6, _excluded$3)), {}, {
		children,
		innerProps
	}), "noOptionsMessage", {
		"menu-notice": true,
		"menu-notice--no-options": true
	}), innerProps), children);
};
var LoadingMessage = function LoadingMessage(_ref7) {
	var _ref7$children = _ref7.children, children = _ref7$children === void 0 ? "Loading..." : _ref7$children, innerProps = _ref7.innerProps;
	return jsx("div", _extends({}, getStyleProps(_objectSpread2(_objectSpread2({}, _objectWithoutProperties(_ref7, _excluded2$1)), {}, {
		children,
		innerProps
	}), "loadingMessage", {
		"menu-notice": true,
		"menu-notice--loading": true
	}), innerProps), children);
};
var menuPortalCSS = function menuPortalCSS(_ref8) {
	var rect = _ref8.rect, offset = _ref8.offset, position = _ref8.position;
	return {
		left: rect.left,
		position,
		top: offset,
		width: rect.width,
		zIndex: 1
	};
};
var MenuPortal = function MenuPortal(props) {
	var appendTo = props.appendTo, children = props.children, controlElement = props.controlElement, innerProps = props.innerProps, menuPlacement = props.menuPlacement, menuPosition = props.menuPosition;
	var menuPortalRef = (0, import_react.useRef)(null);
	var cleanupRef = (0, import_react.useRef)(null);
	var _useState6 = _slicedToArray((0, import_react.useState)(coercePlacement(menuPlacement)), 2), placement = _useState6[0], setPortalPlacement = _useState6[1];
	var portalPlacementContext = (0, import_react.useMemo)(function() {
		return { setPortalPlacement };
	}, []);
	var _useState8 = _slicedToArray((0, import_react.useState)(null), 2), computedPosition = _useState8[0], setComputedPosition = _useState8[1];
	var updateComputedPosition = (0, import_react.useCallback)(function() {
		if (!controlElement) return;
		var rect = getBoundingClientObj(controlElement);
		var scrollDistance = menuPosition === "fixed" ? 0 : window.pageYOffset;
		var offset = rect[placement] + scrollDistance;
		if (offset !== (computedPosition === null || computedPosition === void 0 ? void 0 : computedPosition.offset) || rect.left !== (computedPosition === null || computedPosition === void 0 ? void 0 : computedPosition.rect.left) || rect.width !== (computedPosition === null || computedPosition === void 0 ? void 0 : computedPosition.rect.width)) setComputedPosition({
			offset,
			rect
		});
	}, [
		controlElement,
		menuPosition,
		placement,
		computedPosition === null || computedPosition === void 0 ? void 0 : computedPosition.offset,
		computedPosition === null || computedPosition === void 0 ? void 0 : computedPosition.rect.left,
		computedPosition === null || computedPosition === void 0 ? void 0 : computedPosition.rect.width
	]);
	index(function() {
		updateComputedPosition();
	}, [updateComputedPosition]);
	var runAutoUpdate = (0, import_react.useCallback)(function() {
		if (typeof cleanupRef.current === "function") {
			cleanupRef.current();
			cleanupRef.current = null;
		}
		if (controlElement && menuPortalRef.current) cleanupRef.current = autoUpdate(controlElement, menuPortalRef.current, updateComputedPosition, { elementResize: "ResizeObserver" in window });
	}, [controlElement, updateComputedPosition]);
	index(function() {
		runAutoUpdate();
	}, [runAutoUpdate]);
	var setMenuPortalElement = (0, import_react.useCallback)(function(menuPortalElement) {
		menuPortalRef.current = menuPortalElement;
		runAutoUpdate();
	}, [runAutoUpdate]);
	if (!appendTo && menuPosition !== "fixed" || !computedPosition) return null;
	var menuWrapper = jsx("div", _extends({ ref: setMenuPortalElement }, getStyleProps(_objectSpread2(_objectSpread2({}, props), {}, {
		offset: computedPosition.offset,
		position: menuPosition,
		rect: computedPosition.rect
	}), "menuPortal", { "menu-portal": true }), innerProps), children);
	return jsx(PortalPlacementContext.Provider, { value: portalPlacementContext }, appendTo ? /*#__PURE__*/ (0, import_react_dom.createPortal)(menuWrapper, appendTo) : menuWrapper);
};
var containerCSS = function containerCSS(_ref) {
	var isDisabled = _ref.isDisabled;
	return {
		label: "container",
		direction: _ref.isRtl ? "rtl" : void 0,
		pointerEvents: isDisabled ? "none" : void 0,
		position: "relative"
	};
};
var SelectContainer = function SelectContainer(props) {
	var children = props.children, innerProps = props.innerProps, isDisabled = props.isDisabled, isRtl = props.isRtl;
	return jsx("div", _extends({}, getStyleProps(props, "container", {
		"--is-disabled": isDisabled,
		"--is-rtl": isRtl
	}), innerProps), children);
};
var valueContainerCSS = function valueContainerCSS(_ref2, unstyled) {
	var spacing = _ref2.theme.spacing, isMulti = _ref2.isMulti, hasValue = _ref2.hasValue, controlShouldRenderValue = _ref2.selectProps.controlShouldRenderValue;
	return _objectSpread2({
		alignItems: "center",
		display: isMulti && hasValue && controlShouldRenderValue ? "flex" : "grid",
		flex: 1,
		flexWrap: "wrap",
		WebkitOverflowScrolling: "touch",
		position: "relative",
		overflow: "hidden"
	}, unstyled ? {} : { padding: "".concat(spacing.baseUnit / 2, "px ").concat(spacing.baseUnit * 2, "px") });
};
var ValueContainer = function ValueContainer(props) {
	var children = props.children, innerProps = props.innerProps, isMulti = props.isMulti, hasValue = props.hasValue;
	return jsx("div", _extends({}, getStyleProps(props, "valueContainer", {
		"value-container": true,
		"value-container--is-multi": isMulti,
		"value-container--has-value": hasValue
	}), innerProps), children);
};
var indicatorsContainerCSS = function indicatorsContainerCSS() {
	return {
		alignItems: "center",
		alignSelf: "stretch",
		display: "flex",
		flexShrink: 0
	};
};
var IndicatorsContainer = function IndicatorsContainer(props) {
	var children = props.children, innerProps = props.innerProps;
	return jsx("div", _extends({}, getStyleProps(props, "indicatorsContainer", { indicators: true }), innerProps), children);
};
var _templateObject;
var _excluded$2 = ["size"], _excluded2 = [
	"innerProps",
	"isRtl",
	"size"
];
var _ref2$2 = {
	name: "8mmkcg",
	styles: "display:inline-block;fill:currentColor;line-height:1;stroke:currentColor;stroke-width:0"
};
var Svg = function Svg(_ref) {
	var size = _ref.size, props = _objectWithoutProperties(_ref, _excluded$2);
	return jsx("svg", _extends({
		height: size,
		width: size,
		viewBox: "0 0 20 20",
		"aria-hidden": "true",
		focusable: "false",
		css: _ref2$2
	}, props));
};
var CrossIcon = function CrossIcon(props) {
	return jsx(Svg, _extends({ size: 20 }, props), jsx("path", { d: "M14.348 14.849c-0.469 0.469-1.229 0.469-1.697 0l-2.651-3.030-2.651 3.029c-0.469 0.469-1.229 0.469-1.697 0-0.469-0.469-0.469-1.229 0-1.697l2.758-3.15-2.759-3.152c-0.469-0.469-0.469-1.228 0-1.697s1.228-0.469 1.697 0l2.652 3.031 2.651-3.031c0.469-0.469 1.228-0.469 1.697 0s0.469 1.229 0 1.697l-2.758 3.152 2.758 3.15c0.469 0.469 0.469 1.229 0 1.698z" }));
};
var DownChevron = function DownChevron(props) {
	return jsx(Svg, _extends({ size: 20 }, props), jsx("path", { d: "M4.516 7.548c0.436-0.446 1.043-0.481 1.576 0l3.908 3.747 3.908-3.747c0.533-0.481 1.141-0.446 1.574 0 0.436 0.445 0.408 1.197 0 1.615-0.406 0.418-4.695 4.502-4.695 4.502-0.217 0.223-0.502 0.335-0.787 0.335s-0.57-0.112-0.789-0.335c0 0-4.287-4.084-4.695-4.502s-0.436-1.17 0-1.615z" }));
};
var baseCSS = function baseCSS(_ref3, unstyled) {
	var isFocused = _ref3.isFocused, _ref3$theme = _ref3.theme, baseUnit = _ref3$theme.spacing.baseUnit, colors = _ref3$theme.colors;
	return _objectSpread2({
		label: "indicatorContainer",
		display: "flex",
		transition: "color 150ms"
	}, unstyled ? {} : {
		color: isFocused ? colors.neutral60 : colors.neutral20,
		padding: baseUnit * 2,
		":hover": { color: isFocused ? colors.neutral80 : colors.neutral40 }
	});
};
var dropdownIndicatorCSS = baseCSS;
var DropdownIndicator = function DropdownIndicator(props) {
	var children = props.children, innerProps = props.innerProps;
	return jsx("div", _extends({}, getStyleProps(props, "dropdownIndicator", {
		indicator: true,
		"dropdown-indicator": true
	}), innerProps), children || jsx(DownChevron, null));
};
var clearIndicatorCSS = baseCSS;
var ClearIndicator = function ClearIndicator(props) {
	var children = props.children, innerProps = props.innerProps;
	return jsx("div", _extends({}, getStyleProps(props, "clearIndicator", {
		indicator: true,
		"clear-indicator": true
	}), innerProps), children || jsx(CrossIcon, null));
};
var indicatorSeparatorCSS = function indicatorSeparatorCSS(_ref4, unstyled) {
	var isDisabled = _ref4.isDisabled, _ref4$theme = _ref4.theme, baseUnit = _ref4$theme.spacing.baseUnit, colors = _ref4$theme.colors;
	return _objectSpread2({
		label: "indicatorSeparator",
		alignSelf: "stretch",
		width: 1
	}, unstyled ? {} : {
		backgroundColor: isDisabled ? colors.neutral10 : colors.neutral20,
		marginBottom: baseUnit * 2,
		marginTop: baseUnit * 2
	});
};
var IndicatorSeparator = function IndicatorSeparator(props) {
	var innerProps = props.innerProps;
	return jsx("span", _extends({}, innerProps, getStyleProps(props, "indicatorSeparator", { "indicator-separator": true })));
};
var loadingDotAnimations = keyframes(_templateObject || (_templateObject = _taggedTemplateLiteral(["\n  0%, 80%, 100% { opacity: 0; }\n  40% { opacity: 1; }\n"])));
var loadingIndicatorCSS = function loadingIndicatorCSS(_ref5, unstyled) {
	var isFocused = _ref5.isFocused, size = _ref5.size, _ref5$theme = _ref5.theme, colors = _ref5$theme.colors, baseUnit = _ref5$theme.spacing.baseUnit;
	return _objectSpread2({
		label: "loadingIndicator",
		display: "flex",
		transition: "color 150ms",
		alignSelf: "center",
		fontSize: size,
		lineHeight: 1,
		marginRight: size,
		textAlign: "center",
		verticalAlign: "middle"
	}, unstyled ? {} : {
		color: isFocused ? colors.neutral60 : colors.neutral20,
		padding: baseUnit * 2
	});
};
var LoadingDot = function LoadingDot(_ref6) {
	var delay = _ref6.delay, offset = _ref6.offset;
	return jsx("span", { css: /*#__PURE__*/ css$2({
		animation: "".concat(loadingDotAnimations, " 1s ease-in-out ").concat(delay, "ms infinite;"),
		backgroundColor: "currentColor",
		borderRadius: "1em",
		display: "inline-block",
		marginLeft: offset ? "1em" : void 0,
		height: "1em",
		verticalAlign: "top",
		width: "1em"
	}, "", "") });
};
var LoadingIndicator = function LoadingIndicator(_ref7) {
	var innerProps = _ref7.innerProps, isRtl = _ref7.isRtl, _ref7$size = _ref7.size, size = _ref7$size === void 0 ? 4 : _ref7$size;
	return jsx("div", _extends({}, getStyleProps(_objectSpread2(_objectSpread2({}, _objectWithoutProperties(_ref7, _excluded2)), {}, {
		innerProps,
		isRtl,
		size
	}), "loadingIndicator", {
		indicator: true,
		"loading-indicator": true
	}), innerProps), jsx(LoadingDot, {
		delay: 0,
		offset: isRtl
	}), jsx(LoadingDot, {
		delay: 160,
		offset: true
	}), jsx(LoadingDot, {
		delay: 320,
		offset: !isRtl
	}));
};
var css$1 = function css(_ref, unstyled) {
	var isDisabled = _ref.isDisabled, isFocused = _ref.isFocused, _ref$theme = _ref.theme, colors = _ref$theme.colors, borderRadius = _ref$theme.borderRadius, spacing = _ref$theme.spacing;
	return _objectSpread2({
		label: "control",
		alignItems: "center",
		cursor: "default",
		display: "flex",
		flexWrap: "wrap",
		justifyContent: "space-between",
		minHeight: spacing.controlHeight,
		outline: "0 !important",
		position: "relative",
		transition: "all 100ms"
	}, unstyled ? {} : {
		backgroundColor: isDisabled ? colors.neutral5 : colors.neutral0,
		borderColor: isDisabled ? colors.neutral10 : isFocused ? colors.primary : colors.neutral20,
		borderRadius,
		borderStyle: "solid",
		borderWidth: 1,
		boxShadow: isFocused ? "0 0 0 1px ".concat(colors.primary) : void 0,
		"&:hover": { borderColor: isFocused ? colors.primary : colors.neutral30 }
	});
};
var Control$1 = function Control(props) {
	var children = props.children, isDisabled = props.isDisabled, isFocused = props.isFocused, innerRef = props.innerRef, innerProps = props.innerProps, menuIsOpen = props.menuIsOpen;
	return jsx("div", _extends({ ref: innerRef }, getStyleProps(props, "control", {
		control: true,
		"control--is-disabled": isDisabled,
		"control--is-focused": isFocused,
		"control--menu-is-open": menuIsOpen
	}), innerProps, { "aria-disabled": isDisabled || void 0 }), children);
};
var _excluded$1 = ["data"];
var groupCSS = function groupCSS(_ref, unstyled) {
	var spacing = _ref.theme.spacing;
	return unstyled ? {} : {
		paddingBottom: spacing.baseUnit * 2,
		paddingTop: spacing.baseUnit * 2
	};
};
var Group = function Group(props) {
	var children = props.children, cx = props.cx, getStyles = props.getStyles, getClassNames = props.getClassNames, Heading = props.Heading, headingProps = props.headingProps, innerProps = props.innerProps, label = props.label, theme = props.theme, selectProps = props.selectProps;
	return jsx("div", _extends({}, getStyleProps(props, "group", { group: true }), innerProps), jsx(Heading, _extends({}, headingProps, {
		selectProps,
		theme,
		getStyles,
		getClassNames,
		cx
	}), label), jsx("div", null, children));
};
var groupHeadingCSS = function groupHeadingCSS(_ref2, unstyled) {
	var _ref2$theme = _ref2.theme, colors = _ref2$theme.colors, spacing = _ref2$theme.spacing;
	return _objectSpread2({
		label: "group",
		cursor: "default",
		display: "block"
	}, unstyled ? {} : {
		color: colors.neutral40,
		fontSize: "75%",
		fontWeight: 500,
		marginBottom: "0.25em",
		paddingLeft: spacing.baseUnit * 3,
		paddingRight: spacing.baseUnit * 3,
		textTransform: "uppercase"
	});
};
var GroupHeading = function GroupHeading(props) {
	var _cleanCommonProps = cleanCommonProps(props);
	_cleanCommonProps.data;
	var innerProps = _objectWithoutProperties(_cleanCommonProps, _excluded$1);
	return jsx("div", _extends({}, getStyleProps(props, "groupHeading", { "group-heading": true }), innerProps));
};
var Group$1 = Group;
var _excluded$5 = [
	"innerRef",
	"isDisabled",
	"isHidden",
	"inputClassName"
];
var inputCSS = function inputCSS(_ref, unstyled) {
	var isDisabled = _ref.isDisabled, value = _ref.value, _ref$theme = _ref.theme, spacing = _ref$theme.spacing, colors = _ref$theme.colors;
	return _objectSpread2(_objectSpread2({
		visibility: isDisabled ? "hidden" : "visible",
		transform: value ? "translateZ(0)" : ""
	}, containerStyle), unstyled ? {} : {
		margin: spacing.baseUnit / 2,
		paddingBottom: spacing.baseUnit / 2,
		paddingTop: spacing.baseUnit / 2,
		color: colors.neutral80
	});
};
var spacingStyle = {
	gridArea: "1 / 2",
	font: "inherit",
	minWidth: "2px",
	border: 0,
	margin: 0,
	outline: 0,
	padding: 0
};
var containerStyle = {
	flex: "1 1 auto",
	display: "inline-grid",
	gridArea: "1 / 1 / 2 / 3",
	gridTemplateColumns: "0 min-content",
	"&:after": _objectSpread2({
		content: "attr(data-value) \" \"",
		visibility: "hidden",
		whiteSpace: "pre"
	}, spacingStyle)
};
var inputStyle = function inputStyle(isHidden) {
	return _objectSpread2({
		label: "input",
		color: "inherit",
		background: 0,
		opacity: isHidden ? 0 : 1,
		width: "100%"
	}, spacingStyle);
};
var Input$1 = function Input(props) {
	var cx = props.cx, value = props.value;
	var _cleanCommonProps = cleanCommonProps(props), innerRef = _cleanCommonProps.innerRef, isDisabled = _cleanCommonProps.isDisabled, isHidden = _cleanCommonProps.isHidden, inputClassName = _cleanCommonProps.inputClassName, innerProps = _objectWithoutProperties(_cleanCommonProps, _excluded$5);
	return jsx("div", _extends({}, getStyleProps(props, "input", { "input-container": true }), { "data-value": value || "" }), jsx("input", _extends({
		className: cx({ input: true }, inputClassName),
		ref: innerRef,
		style: inputStyle(isHidden),
		disabled: isDisabled
	}, innerProps)));
};
var multiValueCSS = function multiValueCSS(_ref, unstyled) {
	var _ref$theme = _ref.theme, spacing = _ref$theme.spacing, borderRadius = _ref$theme.borderRadius, colors = _ref$theme.colors;
	return _objectSpread2({
		label: "multiValue",
		display: "flex",
		minWidth: 0
	}, unstyled ? {} : {
		backgroundColor: colors.neutral10,
		borderRadius: borderRadius / 2,
		margin: spacing.baseUnit / 2
	});
};
var multiValueLabelCSS = function multiValueLabelCSS(_ref2, unstyled) {
	var _ref2$theme = _ref2.theme, borderRadius = _ref2$theme.borderRadius, colors = _ref2$theme.colors, cropWithEllipsis = _ref2.cropWithEllipsis;
	return _objectSpread2({
		overflow: "hidden",
		textOverflow: cropWithEllipsis || cropWithEllipsis === void 0 ? "ellipsis" : void 0,
		whiteSpace: "nowrap"
	}, unstyled ? {} : {
		borderRadius: borderRadius / 2,
		color: colors.neutral80,
		fontSize: "85%",
		padding: 3,
		paddingLeft: 6
	});
};
var multiValueRemoveCSS = function multiValueRemoveCSS(_ref3, unstyled) {
	var _ref3$theme = _ref3.theme, spacing = _ref3$theme.spacing, borderRadius = _ref3$theme.borderRadius, colors = _ref3$theme.colors, isFocused = _ref3.isFocused;
	return _objectSpread2({
		alignItems: "center",
		display: "flex"
	}, unstyled ? {} : {
		borderRadius: borderRadius / 2,
		backgroundColor: isFocused ? colors.dangerLight : void 0,
		paddingLeft: spacing.baseUnit,
		paddingRight: spacing.baseUnit,
		":hover": {
			backgroundColor: colors.dangerLight,
			color: colors.danger
		}
	});
};
var MultiValueGeneric = function MultiValueGeneric(_ref4) {
	var children = _ref4.children, innerProps = _ref4.innerProps;
	return jsx("div", innerProps, children);
};
var MultiValueContainer = MultiValueGeneric;
var MultiValueLabel = MultiValueGeneric;
function MultiValueRemove(_ref5) {
	var children = _ref5.children, innerProps = _ref5.innerProps;
	return jsx("div", _extends({ role: "button" }, innerProps), children || jsx(CrossIcon, { size: 14 }));
}
var MultiValue$1 = function MultiValue(props) {
	var children = props.children, components = props.components, data = props.data, innerProps = props.innerProps, isDisabled = props.isDisabled, removeProps = props.removeProps, selectProps = props.selectProps;
	var Container = components.Container, Label = components.Label, Remove = components.Remove;
	return jsx(Container, {
		data,
		innerProps: _objectSpread2(_objectSpread2({}, getStyleProps(props, "multiValue", {
			"multi-value": true,
			"multi-value--is-disabled": isDisabled
		})), innerProps),
		selectProps
	}, jsx(Label, {
		data,
		innerProps: _objectSpread2({}, getStyleProps(props, "multiValueLabel", { "multi-value__label": true })),
		selectProps
	}, children), jsx(Remove, {
		data,
		innerProps: _objectSpread2(_objectSpread2({}, getStyleProps(props, "multiValueRemove", { "multi-value__remove": true })), {}, { "aria-label": "Remove ".concat(children || "option") }, removeProps),
		selectProps
	}));
};
var optionCSS = function optionCSS(_ref, unstyled) {
	var isDisabled = _ref.isDisabled, isFocused = _ref.isFocused, isSelected = _ref.isSelected, _ref$theme = _ref.theme, spacing = _ref$theme.spacing, colors = _ref$theme.colors;
	return _objectSpread2({
		label: "option",
		cursor: "default",
		display: "block",
		fontSize: "inherit",
		width: "100%",
		userSelect: "none",
		WebkitTapHighlightColor: "rgba(0, 0, 0, 0)"
	}, unstyled ? {} : {
		backgroundColor: isSelected ? colors.primary : isFocused ? colors.primary25 : "transparent",
		color: isDisabled ? colors.neutral20 : isSelected ? colors.neutral0 : "inherit",
		padding: "".concat(spacing.baseUnit * 2, "px ").concat(spacing.baseUnit * 3, "px"),
		":active": { backgroundColor: !isDisabled ? isSelected ? colors.primary : colors.primary50 : void 0 }
	});
};
var Option$1 = function Option(props) {
	var children = props.children, isDisabled = props.isDisabled, isFocused = props.isFocused, isSelected = props.isSelected, innerRef = props.innerRef, innerProps = props.innerProps;
	return jsx("div", _extends({}, getStyleProps(props, "option", {
		option: true,
		"option--is-disabled": isDisabled,
		"option--is-focused": isFocused,
		"option--is-selected": isSelected
	}), {
		ref: innerRef,
		"aria-disabled": isDisabled
	}, innerProps), children);
};
var placeholderCSS = function placeholderCSS(_ref, unstyled) {
	var _ref$theme = _ref.theme, spacing = _ref$theme.spacing, colors = _ref$theme.colors;
	return _objectSpread2({
		label: "placeholder",
		gridArea: "1 / 1 / 2 / 3"
	}, unstyled ? {} : {
		color: colors.neutral50,
		marginLeft: spacing.baseUnit / 2,
		marginRight: spacing.baseUnit / 2
	});
};
var Placeholder$1 = function Placeholder(props) {
	var children = props.children, innerProps = props.innerProps;
	return jsx("div", _extends({}, getStyleProps(props, "placeholder", { placeholder: true }), innerProps), children);
};
var css = function css(_ref, unstyled) {
	var isDisabled = _ref.isDisabled, _ref$theme = _ref.theme, spacing = _ref$theme.spacing, colors = _ref$theme.colors;
	return _objectSpread2({
		label: "singleValue",
		gridArea: "1 / 1 / 2 / 3",
		maxWidth: "100%",
		overflow: "hidden",
		textOverflow: "ellipsis",
		whiteSpace: "nowrap"
	}, unstyled ? {} : {
		color: isDisabled ? colors.neutral40 : colors.neutral80,
		marginLeft: spacing.baseUnit / 2,
		marginRight: spacing.baseUnit / 2
	});
};
var components = {
	ClearIndicator,
	Control: Control$1,
	DropdownIndicator,
	DownChevron,
	CrossIcon,
	Group: Group$1,
	GroupHeading,
	IndicatorsContainer,
	IndicatorSeparator,
	Input: Input$1,
	LoadingIndicator,
	Menu: Menu$1,
	MenuList,
	MenuPortal,
	LoadingMessage,
	NoOptionsMessage,
	MultiValue: MultiValue$1,
	MultiValueContainer,
	MultiValueLabel,
	MultiValueRemove,
	Option: Option$1,
	Placeholder: Placeholder$1,
	SelectContainer,
	SingleValue: function SingleValue(props) {
		var children = props.children, isDisabled = props.isDisabled, innerProps = props.innerProps;
		return jsx("div", _extends({}, getStyleProps(props, "singleValue", {
			"single-value": true,
			"single-value--is-disabled": isDisabled
		}), innerProps), children);
	},
	ValueContainer
};
var defaultComponents = function defaultComponents(props) {
	return _objectSpread2(_objectSpread2({}, components), props.components);
};
//#endregion
//#region node_modules/memoize-one/dist/memoize-one.esm.js
var safeIsNaN = Number.isNaN || function ponyfill(value) {
	return typeof value === "number" && value !== value;
};
function isEqual(first, second) {
	if (first === second) return true;
	if (safeIsNaN(first) && safeIsNaN(second)) return true;
	return false;
}
function areInputsEqual(newInputs, lastInputs) {
	if (newInputs.length !== lastInputs.length) return false;
	for (var i = 0; i < newInputs.length; i++) if (!isEqual(newInputs[i], lastInputs[i])) return false;
	return true;
}
function memoizeOne(resultFn, isEqual) {
	if (isEqual === void 0) isEqual = areInputsEqual;
	var cache = null;
	function memoized() {
		var newArgs = [];
		for (var _i = 0; _i < arguments.length; _i++) newArgs[_i] = arguments[_i];
		if (cache && cache.lastThis === this && isEqual(newArgs, cache.lastArgs)) return cache.lastResult;
		var lastResult = resultFn.apply(this, newArgs);
		cache = {
			lastResult,
			lastArgs: newArgs,
			lastThis: this
		};
		return lastResult;
	}
	memoized.clear = function clear() {
		cache = null;
	};
	return memoized;
}
//#endregion
//#region node_modules/react-select/dist/Select-ef7c0426.esm.js
var _ref = {
	name: "7pg0cj-a11yText",
	styles: "label:a11yText;z-index:9999;border:0;clip:rect(1px, 1px, 1px, 1px);height:1px;width:1px;position:absolute;overflow:hidden;padding:0;white-space:nowrap"
};
var A11yText$1 = function A11yText(props) {
	return jsx("span", _extends({ css: _ref }, props));
};
var defaultAriaLiveMessages = {
	guidance: function guidance(props) {
		var isSearchable = props.isSearchable, isMulti = props.isMulti, tabSelectsValue = props.tabSelectsValue, context = props.context, isInitialFocus = props.isInitialFocus;
		switch (context) {
			case "menu": return "Use Up and Down to choose options, press Enter to select the currently focused option, press Escape to exit the menu".concat(tabSelectsValue ? ", press Tab to select the option and exit the menu" : "", ".");
			case "input": return isInitialFocus ? "".concat(props["aria-label"] || "Select", " is focused ").concat(isSearchable ? ",type to refine list" : "", ", press Down to open the menu, ").concat(isMulti ? " press left to focus selected values" : "") : "";
			case "value": return "Use left and right to toggle between focused values, press Backspace to remove the currently focused value";
			default: return "";
		}
	},
	onChange: function onChange(props) {
		var action = props.action, _props$label = props.label, label = _props$label === void 0 ? "" : _props$label, labels = props.labels, isDisabled = props.isDisabled;
		switch (action) {
			case "deselect-option":
			case "pop-value":
			case "remove-value": return "option ".concat(label, ", deselected.");
			case "clear": return "All selected options have been cleared.";
			case "initial-input-focus": return "option".concat(labels.length > 1 ? "s" : "", " ").concat(labels.join(","), ", selected.");
			case "select-option": return isDisabled ? "option ".concat(label, " is disabled. Select another option.") : "option ".concat(label, ", selected.");
			default: return "";
		}
	},
	onFocus: function onFocus(props) {
		var context = props.context, focused = props.focused, options = props.options, _props$label2 = props.label, label = _props$label2 === void 0 ? "" : _props$label2, selectValue = props.selectValue, isDisabled = props.isDisabled, isSelected = props.isSelected, isAppleDevice = props.isAppleDevice;
		var getArrayIndex = function getArrayIndex(arr, item) {
			return arr && arr.length ? "".concat(arr.indexOf(item) + 1, " of ").concat(arr.length) : "";
		};
		if (context === "value" && selectValue) return "value ".concat(label, " focused, ").concat(getArrayIndex(selectValue, focused), ".");
		if (context === "menu" && isAppleDevice) {
			var disabled = isDisabled ? " disabled" : "";
			var status = "".concat(isSelected ? " selected" : "").concat(disabled);
			return "".concat(label).concat(status, ", ").concat(getArrayIndex(options, focused), ".");
		}
		return "";
	},
	onFilter: function onFilter(props) {
		var inputValue = props.inputValue, resultsMessage = props.resultsMessage;
		return "".concat(resultsMessage).concat(inputValue ? " for search term " + inputValue : "", ".");
	}
};
var LiveRegion$1 = function LiveRegion(props) {
	var ariaSelection = props.ariaSelection, focusedOption = props.focusedOption, focusedValue = props.focusedValue, focusableOptions = props.focusableOptions, isFocused = props.isFocused, selectValue = props.selectValue, selectProps = props.selectProps, id = props.id, isAppleDevice = props.isAppleDevice;
	var ariaLiveMessages = selectProps.ariaLiveMessages, getOptionLabel = selectProps.getOptionLabel, inputValue = selectProps.inputValue, isMulti = selectProps.isMulti, isOptionDisabled = selectProps.isOptionDisabled, isSearchable = selectProps.isSearchable, menuIsOpen = selectProps.menuIsOpen, options = selectProps.options, screenReaderStatus = selectProps.screenReaderStatus, tabSelectsValue = selectProps.tabSelectsValue, isLoading = selectProps.isLoading;
	var ariaLabel = selectProps["aria-label"];
	var ariaLive = selectProps["aria-live"];
	var messages = (0, import_react.useMemo)(function() {
		return _objectSpread2(_objectSpread2({}, defaultAriaLiveMessages), ariaLiveMessages || {});
	}, [ariaLiveMessages]);
	var ariaSelected = (0, import_react.useMemo)(function() {
		var message = "";
		if (ariaSelection && messages.onChange) {
			var option = ariaSelection.option, selectedOptions = ariaSelection.options, removedValue = ariaSelection.removedValue, removedValues = ariaSelection.removedValues, value = ariaSelection.value;
			var selected = removedValue || option || function asOption(val) {
				return !Array.isArray(val) ? val : null;
			}(value);
			var label = selected ? getOptionLabel(selected) : "";
			var multiSelected = selectedOptions || removedValues || void 0;
			var labels = multiSelected ? multiSelected.map(getOptionLabel) : [];
			var onChangeProps = _objectSpread2({
				isDisabled: selected && isOptionDisabled(selected, selectValue),
				label,
				labels
			}, ariaSelection);
			message = messages.onChange(onChangeProps);
		}
		return message;
	}, [
		ariaSelection,
		messages,
		isOptionDisabled,
		selectValue,
		getOptionLabel
	]);
	var ariaFocused = (0, import_react.useMemo)(function() {
		var focusMsg = "";
		var focused = focusedOption || focusedValue;
		var isSelected = !!(focusedOption && selectValue && selectValue.includes(focusedOption));
		if (focused && messages.onFocus) {
			var onFocusProps = {
				focused,
				label: getOptionLabel(focused),
				isDisabled: isOptionDisabled(focused, selectValue),
				isSelected,
				options: focusableOptions,
				context: focused === focusedOption ? "menu" : "value",
				selectValue,
				isAppleDevice
			};
			focusMsg = messages.onFocus(onFocusProps);
		}
		return focusMsg;
	}, [
		focusedOption,
		focusedValue,
		getOptionLabel,
		isOptionDisabled,
		messages,
		focusableOptions,
		selectValue,
		isAppleDevice
	]);
	var ariaResults = (0, import_react.useMemo)(function() {
		var resultsMsg = "";
		if (menuIsOpen && options.length && !isLoading && messages.onFilter) {
			var resultsMessage = screenReaderStatus({ count: focusableOptions.length });
			resultsMsg = messages.onFilter({
				inputValue,
				resultsMessage
			});
		}
		return resultsMsg;
	}, [
		focusableOptions,
		inputValue,
		menuIsOpen,
		messages,
		options,
		screenReaderStatus,
		isLoading
	]);
	var isInitialFocus = (ariaSelection === null || ariaSelection === void 0 ? void 0 : ariaSelection.action) === "initial-input-focus";
	var ariaGuidance = (0, import_react.useMemo)(function() {
		var guidanceMsg = "";
		if (messages.guidance) {
			var context = focusedValue ? "value" : menuIsOpen ? "menu" : "input";
			guidanceMsg = messages.guidance({
				"aria-label": ariaLabel,
				context,
				isDisabled: focusedOption && isOptionDisabled(focusedOption, selectValue),
				isMulti,
				isSearchable,
				tabSelectsValue,
				isInitialFocus
			});
		}
		return guidanceMsg;
	}, [
		ariaLabel,
		focusedOption,
		focusedValue,
		isMulti,
		isOptionDisabled,
		isSearchable,
		menuIsOpen,
		messages,
		selectValue,
		tabSelectsValue,
		isInitialFocus
	]);
	var ScreenReaderText = jsx(import_react.Fragment, null, jsx("span", { id: "aria-selection" }, ariaSelected), jsx("span", { id: "aria-focused" }, ariaFocused), jsx("span", { id: "aria-results" }, ariaResults), jsx("span", { id: "aria-guidance" }, ariaGuidance));
	return jsx(import_react.Fragment, null, jsx(A11yText$1, { id }, isInitialFocus && ScreenReaderText), jsx(A11yText$1, {
		"aria-live": ariaLive,
		"aria-atomic": "false",
		"aria-relevant": "additions text",
		role: "log"
	}, isFocused && !isInitialFocus && ScreenReaderText));
};
var diacritics = [
	{
		base: "A",
		letters: "AⒶＡÀÁÂẦẤẪẨÃĀĂẰẮẴẲȦǠÄǞẢÅǺǍȀȂẠẬẶḀĄȺⱯ"
	},
	{
		base: "AA",
		letters: "Ꜳ"
	},
	{
		base: "AE",
		letters: "ÆǼǢ"
	},
	{
		base: "AO",
		letters: "Ꜵ"
	},
	{
		base: "AU",
		letters: "Ꜷ"
	},
	{
		base: "AV",
		letters: "ꜸꜺ"
	},
	{
		base: "AY",
		letters: "Ꜽ"
	},
	{
		base: "B",
		letters: "BⒷＢḂḄḆɃƂƁ"
	},
	{
		base: "C",
		letters: "CⒸＣĆĈĊČÇḈƇȻꜾ"
	},
	{
		base: "D",
		letters: "DⒹＤḊĎḌḐḒḎĐƋƊƉꝹ"
	},
	{
		base: "DZ",
		letters: "ǱǄ"
	},
	{
		base: "Dz",
		letters: "ǲǅ"
	},
	{
		base: "E",
		letters: "EⒺＥÈÉÊỀẾỄỂẼĒḔḖĔĖËẺĚȄȆẸỆȨḜĘḘḚƐƎ"
	},
	{
		base: "F",
		letters: "FⒻＦḞƑꝻ"
	},
	{
		base: "G",
		letters: "GⒼＧǴĜḠĞĠǦĢǤƓꞠꝽꝾ"
	},
	{
		base: "H",
		letters: "HⒽＨĤḢḦȞḤḨḪĦⱧⱵꞍ"
	},
	{
		base: "I",
		letters: "IⒾＩÌÍÎĨĪĬİÏḮỈǏȈȊỊĮḬƗ"
	},
	{
		base: "J",
		letters: "JⒿＪĴɈ"
	},
	{
		base: "K",
		letters: "KⓀＫḰǨḲĶḴƘⱩꝀꝂꝄꞢ"
	},
	{
		base: "L",
		letters: "LⓁＬĿĹĽḶḸĻḼḺŁȽⱢⱠꝈꝆꞀ"
	},
	{
		base: "LJ",
		letters: "Ǉ"
	},
	{
		base: "Lj",
		letters: "ǈ"
	},
	{
		base: "M",
		letters: "MⓂＭḾṀṂⱮƜ"
	},
	{
		base: "N",
		letters: "NⓃＮǸŃÑṄŇṆŅṊṈȠƝꞐꞤ"
	},
	{
		base: "NJ",
		letters: "Ǌ"
	},
	{
		base: "Nj",
		letters: "ǋ"
	},
	{
		base: "O",
		letters: "OⓄＯÒÓÔỒỐỖỔÕṌȬṎŌṐṒŎȮȰÖȪỎŐǑȌȎƠỜỚỠỞỢỌỘǪǬØǾƆƟꝊꝌ"
	},
	{
		base: "OI",
		letters: "Ƣ"
	},
	{
		base: "OO",
		letters: "Ꝏ"
	},
	{
		base: "OU",
		letters: "Ȣ"
	},
	{
		base: "P",
		letters: "PⓅＰṔṖƤⱣꝐꝒꝔ"
	},
	{
		base: "Q",
		letters: "QⓆＱꝖꝘɊ"
	},
	{
		base: "R",
		letters: "RⓇＲŔṘŘȐȒṚṜŖṞɌⱤꝚꞦꞂ"
	},
	{
		base: "S",
		letters: "SⓈＳẞŚṤŜṠŠṦṢṨȘŞⱾꞨꞄ"
	},
	{
		base: "T",
		letters: "TⓉＴṪŤṬȚŢṰṮŦƬƮȾꞆ"
	},
	{
		base: "TZ",
		letters: "Ꜩ"
	},
	{
		base: "U",
		letters: "UⓊＵÙÚÛŨṸŪṺŬÜǛǗǕǙỦŮŰǓȔȖƯỪỨỮỬỰỤṲŲṶṴɄ"
	},
	{
		base: "V",
		letters: "VⓋＶṼṾƲꝞɅ"
	},
	{
		base: "VY",
		letters: "Ꝡ"
	},
	{
		base: "W",
		letters: "WⓌＷẀẂŴẆẄẈⱲ"
	},
	{
		base: "X",
		letters: "XⓍＸẊẌ"
	},
	{
		base: "Y",
		letters: "YⓎＹỲÝŶỸȲẎŸỶỴƳɎỾ"
	},
	{
		base: "Z",
		letters: "ZⓏＺŹẐŻŽẒẔƵȤⱿⱫꝢ"
	},
	{
		base: "a",
		letters: "aⓐａẚàáâầấẫẩãāăằắẵẳȧǡäǟảåǻǎȁȃạậặḁąⱥɐ"
	},
	{
		base: "aa",
		letters: "ꜳ"
	},
	{
		base: "ae",
		letters: "æǽǣ"
	},
	{
		base: "ao",
		letters: "ꜵ"
	},
	{
		base: "au",
		letters: "ꜷ"
	},
	{
		base: "av",
		letters: "ꜹꜻ"
	},
	{
		base: "ay",
		letters: "ꜽ"
	},
	{
		base: "b",
		letters: "bⓑｂḃḅḇƀƃɓ"
	},
	{
		base: "c",
		letters: "cⓒｃćĉċčçḉƈȼꜿↄ"
	},
	{
		base: "d",
		letters: "dⓓｄḋďḍḑḓḏđƌɖɗꝺ"
	},
	{
		base: "dz",
		letters: "ǳǆ"
	},
	{
		base: "e",
		letters: "eⓔｅèéêềếễểẽēḕḗĕėëẻěȅȇẹệȩḝęḙḛɇɛǝ"
	},
	{
		base: "f",
		letters: "fⓕｆḟƒꝼ"
	},
	{
		base: "g",
		letters: "gⓖｇǵĝḡğġǧģǥɠꞡᵹꝿ"
	},
	{
		base: "h",
		letters: "hⓗｈĥḣḧȟḥḩḫẖħⱨⱶɥ"
	},
	{
		base: "hv",
		letters: "ƕ"
	},
	{
		base: "i",
		letters: "iⓘｉìíîĩīĭïḯỉǐȉȋịįḭɨı"
	},
	{
		base: "j",
		letters: "jⓙｊĵǰɉ"
	},
	{
		base: "k",
		letters: "kⓚｋḱǩḳķḵƙⱪꝁꝃꝅꞣ"
	},
	{
		base: "l",
		letters: "lⓛｌŀĺľḷḹļḽḻſłƚɫⱡꝉꞁꝇ"
	},
	{
		base: "lj",
		letters: "ǉ"
	},
	{
		base: "m",
		letters: "mⓜｍḿṁṃɱɯ"
	},
	{
		base: "n",
		letters: "nⓝｎǹńñṅňṇņṋṉƞɲŉꞑꞥ"
	},
	{
		base: "nj",
		letters: "ǌ"
	},
	{
		base: "o",
		letters: "oⓞｏòóôồốỗổõṍȭṏōṑṓŏȯȱöȫỏőǒȍȏơờớỡởợọộǫǭøǿɔꝋꝍɵ"
	},
	{
		base: "oi",
		letters: "ƣ"
	},
	{
		base: "ou",
		letters: "ȣ"
	},
	{
		base: "oo",
		letters: "ꝏ"
	},
	{
		base: "p",
		letters: "pⓟｐṕṗƥᵽꝑꝓꝕ"
	},
	{
		base: "q",
		letters: "qⓠｑɋꝗꝙ"
	},
	{
		base: "r",
		letters: "rⓡｒŕṙřȑȓṛṝŗṟɍɽꝛꞧꞃ"
	},
	{
		base: "s",
		letters: "sⓢｓßśṥŝṡšṧṣṩșşȿꞩꞅẛ"
	},
	{
		base: "t",
		letters: "tⓣｔṫẗťṭțţṱṯŧƭʈⱦꞇ"
	},
	{
		base: "tz",
		letters: "ꜩ"
	},
	{
		base: "u",
		letters: "uⓤｕùúûũṹūṻŭüǜǘǖǚủůűǔȕȗưừứữửựụṳųṷṵʉ"
	},
	{
		base: "v",
		letters: "vⓥｖṽṿʋꝟʌ"
	},
	{
		base: "vy",
		letters: "ꝡ"
	},
	{
		base: "w",
		letters: "wⓦｗẁẃŵẇẅẘẉⱳ"
	},
	{
		base: "x",
		letters: "xⓧｘẋẍ"
	},
	{
		base: "y",
		letters: "yⓨｙỳýŷỹȳẏÿỷẙỵƴɏỿ"
	},
	{
		base: "z",
		letters: "zⓩｚźẑżžẓẕƶȥɀⱬꝣ"
	}
];
var anyDiacritic = new RegExp("[" + diacritics.map(function(d) {
	return d.letters;
}).join("") + "]", "g");
var diacriticToBase = {};
for (var i = 0; i < diacritics.length; i++) {
	var diacritic = diacritics[i];
	for (var j = 0; j < diacritic.letters.length; j++) diacriticToBase[diacritic.letters[j]] = diacritic.base;
}
var stripDiacritics = function stripDiacritics(str) {
	return str.replace(anyDiacritic, function(match) {
		return diacriticToBase[match];
	});
};
var memoizedStripDiacriticsForInput = memoizeOne(stripDiacritics);
var trimString = function trimString(str) {
	return str.replace(/^\s+|\s+$/g, "");
};
var defaultStringify = function defaultStringify(option) {
	return "".concat(option.label, " ").concat(option.value);
};
var createFilter = function createFilter(config) {
	return function(option, rawInput) {
		if (option.data.__isNew__) return true;
		var _ignoreCase$ignoreAcc = _objectSpread2({
			ignoreCase: true,
			ignoreAccents: true,
			stringify: defaultStringify,
			trim: true,
			matchFrom: "any"
		}, config), ignoreCase = _ignoreCase$ignoreAcc.ignoreCase, ignoreAccents = _ignoreCase$ignoreAcc.ignoreAccents, stringify = _ignoreCase$ignoreAcc.stringify, trim = _ignoreCase$ignoreAcc.trim, matchFrom = _ignoreCase$ignoreAcc.matchFrom;
		var input = trim ? trimString(rawInput) : rawInput;
		var candidate = trim ? trimString(stringify(option)) : stringify(option);
		if (ignoreCase) {
			input = input.toLowerCase();
			candidate = candidate.toLowerCase();
		}
		if (ignoreAccents) {
			input = memoizedStripDiacriticsForInput(input);
			candidate = stripDiacritics(candidate);
		}
		return matchFrom === "start" ? candidate.substr(0, input.length) === input : candidate.indexOf(input) > -1;
	};
};
var _excluded = ["innerRef"];
function DummyInput(_ref) {
	var innerRef = _ref.innerRef;
	var filteredProps = removeProps(_objectWithoutProperties(_ref, _excluded), "onExited", "in", "enter", "exit", "appear");
	return jsx("input", _extends({ ref: innerRef }, filteredProps, { css: /*#__PURE__*/ css$2({
		label: "dummyInput",
		background: 0,
		border: 0,
		caretColor: "transparent",
		fontSize: "inherit",
		gridArea: "1 / 1 / 2 / 3",
		outline: 0,
		padding: 0,
		width: 1,
		color: "transparent",
		left: -100,
		opacity: 0,
		position: "relative",
		transform: "scale(.01)"
	}, "", "") }));
}
var cancelScroll = function cancelScroll(event) {
	if (event.cancelable) event.preventDefault();
	event.stopPropagation();
};
function useScrollCapture(_ref) {
	var isEnabled = _ref.isEnabled, onBottomArrive = _ref.onBottomArrive, onBottomLeave = _ref.onBottomLeave, onTopArrive = _ref.onTopArrive, onTopLeave = _ref.onTopLeave;
	var isBottom = (0, import_react.useRef)(false);
	var isTop = (0, import_react.useRef)(false);
	var touchStart = (0, import_react.useRef)(0);
	var scrollTarget = (0, import_react.useRef)(null);
	var handleEventDelta = (0, import_react.useCallback)(function(event, delta) {
		if (scrollTarget.current === null) return;
		var _scrollTarget$current = scrollTarget.current, scrollTop = _scrollTarget$current.scrollTop, scrollHeight = _scrollTarget$current.scrollHeight, clientHeight = _scrollTarget$current.clientHeight;
		var target = scrollTarget.current;
		var isDeltaPositive = delta > 0;
		var availableScroll = scrollHeight - clientHeight - scrollTop;
		var shouldCancelScroll = false;
		if (availableScroll > delta && isBottom.current) {
			if (onBottomLeave) onBottomLeave(event);
			isBottom.current = false;
		}
		if (isDeltaPositive && isTop.current) {
			if (onTopLeave) onTopLeave(event);
			isTop.current = false;
		}
		if (isDeltaPositive && delta > availableScroll) {
			if (onBottomArrive && !isBottom.current) onBottomArrive(event);
			target.scrollTop = scrollHeight;
			shouldCancelScroll = true;
			isBottom.current = true;
		} else if (!isDeltaPositive && -delta > scrollTop) {
			if (onTopArrive && !isTop.current) onTopArrive(event);
			target.scrollTop = 0;
			shouldCancelScroll = true;
			isTop.current = true;
		}
		if (shouldCancelScroll) cancelScroll(event);
	}, [
		onBottomArrive,
		onBottomLeave,
		onTopArrive,
		onTopLeave
	]);
	var onWheel = (0, import_react.useCallback)(function(event) {
		handleEventDelta(event, event.deltaY);
	}, [handleEventDelta]);
	var onTouchStart = (0, import_react.useCallback)(function(event) {
		touchStart.current = event.changedTouches[0].clientY;
	}, []);
	var onTouchMove = (0, import_react.useCallback)(function(event) {
		handleEventDelta(event, touchStart.current - event.changedTouches[0].clientY);
	}, [handleEventDelta]);
	var startListening = (0, import_react.useCallback)(function(el) {
		if (!el) return;
		var notPassive = supportsPassiveEvents ? { passive: false } : false;
		el.addEventListener("wheel", onWheel, notPassive);
		el.addEventListener("touchstart", onTouchStart, notPassive);
		el.addEventListener("touchmove", onTouchMove, notPassive);
	}, [
		onTouchMove,
		onTouchStart,
		onWheel
	]);
	var stopListening = (0, import_react.useCallback)(function(el) {
		if (!el) return;
		el.removeEventListener("wheel", onWheel, false);
		el.removeEventListener("touchstart", onTouchStart, false);
		el.removeEventListener("touchmove", onTouchMove, false);
	}, [
		onTouchMove,
		onTouchStart,
		onWheel
	]);
	(0, import_react.useEffect)(function() {
		if (!isEnabled) return;
		var element = scrollTarget.current;
		startListening(element);
		return function() {
			stopListening(element);
		};
	}, [
		isEnabled,
		startListening,
		stopListening
	]);
	return function(element) {
		scrollTarget.current = element;
	};
}
var STYLE_KEYS = [
	"boxSizing",
	"height",
	"overflow",
	"paddingRight",
	"position"
];
var LOCK_STYLES = {
	boxSizing: "border-box",
	overflow: "hidden",
	position: "relative",
	height: "100%"
};
function preventTouchMove(e) {
	if (e.cancelable) e.preventDefault();
}
function allowTouchMove(e) {
	e.stopPropagation();
}
function preventInertiaScroll() {
	var top = this.scrollTop;
	var totalScroll = this.scrollHeight;
	var currentScroll = top + this.offsetHeight;
	if (top === 0) this.scrollTop = 1;
	else if (currentScroll === totalScroll) this.scrollTop = top - 1;
}
function isTouchDevice() {
	return "ontouchstart" in window || navigator.maxTouchPoints;
}
var canUseDOM = !!(typeof window !== "undefined" && window.document && window.document.createElement);
var activeScrollLocks = 0;
var listenerOptions = {
	capture: false,
	passive: false
};
function useScrollLock(_ref) {
	var isEnabled = _ref.isEnabled, _ref$accountForScroll = _ref.accountForScrollbars, accountForScrollbars = _ref$accountForScroll === void 0 ? true : _ref$accountForScroll;
	var originalStyles = (0, import_react.useRef)({});
	var scrollTarget = (0, import_react.useRef)(null);
	var addScrollLock = (0, import_react.useCallback)(function(touchScrollTarget) {
		if (!canUseDOM) return;
		var target = document.body;
		var targetStyle = target && target.style;
		if (accountForScrollbars) STYLE_KEYS.forEach(function(key) {
			var val = targetStyle && targetStyle[key];
			originalStyles.current[key] = val;
		});
		if (accountForScrollbars && activeScrollLocks < 1) {
			var currentPadding = parseInt(originalStyles.current.paddingRight, 10) || 0;
			var clientWidth = document.body ? document.body.clientWidth : 0;
			var adjustedPadding = window.innerWidth - clientWidth + currentPadding || 0;
			Object.keys(LOCK_STYLES).forEach(function(key) {
				var val = LOCK_STYLES[key];
				if (targetStyle) targetStyle[key] = val;
			});
			if (targetStyle) targetStyle.paddingRight = "".concat(adjustedPadding, "px");
		}
		if (target && isTouchDevice()) {
			target.addEventListener("touchmove", preventTouchMove, listenerOptions);
			if (touchScrollTarget) {
				touchScrollTarget.addEventListener("touchstart", preventInertiaScroll, listenerOptions);
				touchScrollTarget.addEventListener("touchmove", allowTouchMove, listenerOptions);
			}
		}
		activeScrollLocks += 1;
	}, [accountForScrollbars]);
	var removeScrollLock = (0, import_react.useCallback)(function(touchScrollTarget) {
		if (!canUseDOM) return;
		var target = document.body;
		var targetStyle = target && target.style;
		activeScrollLocks = Math.max(activeScrollLocks - 1, 0);
		if (accountForScrollbars && activeScrollLocks < 1) STYLE_KEYS.forEach(function(key) {
			var val = originalStyles.current[key];
			if (targetStyle) targetStyle[key] = val;
		});
		if (target && isTouchDevice()) {
			target.removeEventListener("touchmove", preventTouchMove, listenerOptions);
			if (touchScrollTarget) {
				touchScrollTarget.removeEventListener("touchstart", preventInertiaScroll, listenerOptions);
				touchScrollTarget.removeEventListener("touchmove", allowTouchMove, listenerOptions);
			}
		}
	}, [accountForScrollbars]);
	(0, import_react.useEffect)(function() {
		if (!isEnabled) return;
		var element = scrollTarget.current;
		addScrollLock(element);
		return function() {
			removeScrollLock(element);
		};
	}, [
		isEnabled,
		addScrollLock,
		removeScrollLock
	]);
	return function(element) {
		scrollTarget.current = element;
	};
}
var blurSelectInput = function blurSelectInput(event) {
	var element = event.target;
	return element.ownerDocument.activeElement && element.ownerDocument.activeElement.blur();
};
var _ref2$1 = {
	name: "1kfdb0e",
	styles: "position:fixed;left:0;bottom:0;right:0;top:0"
};
function ScrollManager(_ref) {
	var children = _ref.children, lockEnabled = _ref.lockEnabled, _ref$captureEnabled = _ref.captureEnabled, captureEnabled = _ref$captureEnabled === void 0 ? true : _ref$captureEnabled, onBottomArrive = _ref.onBottomArrive, onBottomLeave = _ref.onBottomLeave, onTopArrive = _ref.onTopArrive, onTopLeave = _ref.onTopLeave;
	var setScrollCaptureTarget = useScrollCapture({
		isEnabled: captureEnabled,
		onBottomArrive,
		onBottomLeave,
		onTopArrive,
		onTopLeave
	});
	var setScrollLockTarget = useScrollLock({ isEnabled: lockEnabled });
	return jsx(import_react.Fragment, null, lockEnabled && jsx("div", {
		onClick: blurSelectInput,
		css: _ref2$1
	}), children(function targetRef(element) {
		setScrollCaptureTarget(element);
		setScrollLockTarget(element);
	}));
}
var _ref2 = {
	name: "1a0ro4n-requiredInput",
	styles: "label:requiredInput;opacity:0;pointer-events:none;position:absolute;bottom:0;left:0;right:0;width:100%"
};
var RequiredInput$1 = function RequiredInput(_ref) {
	var name = _ref.name, onFocus = _ref.onFocus;
	return jsx("input", {
		required: true,
		name,
		tabIndex: -1,
		"aria-hidden": "true",
		onFocus,
		css: _ref2,
		value: "",
		onChange: function onChange() {}
	});
};
function testPlatform(re) {
	var _window$navigator$use;
	return typeof window !== "undefined" && window.navigator != null ? re.test(((_window$navigator$use = window.navigator["userAgentData"]) === null || _window$navigator$use === void 0 ? void 0 : _window$navigator$use.platform) || window.navigator.platform) : false;
}
function isIPhone() {
	return testPlatform(/^iPhone/i);
}
function isMac() {
	return testPlatform(/^Mac/i);
}
function isIPad() {
	return testPlatform(/^iPad/i) || isMac() && navigator.maxTouchPoints > 1;
}
function isIOS() {
	return isIPhone() || isIPad();
}
function isAppleDevice() {
	return isMac() || isIOS();
}
var formatGroupLabel = function formatGroupLabel(group) {
	return group.label;
};
var getOptionLabel$1 = function getOptionLabel(option) {
	return option.label;
};
var getOptionValue$1 = function getOptionValue(option) {
	return option.value;
};
var isOptionDisabled = function isOptionDisabled(option) {
	return !!option.isDisabled;
};
var defaultStyles = {
	clearIndicator: clearIndicatorCSS,
	container: containerCSS,
	control: css$1,
	dropdownIndicator: dropdownIndicatorCSS,
	group: groupCSS,
	groupHeading: groupHeadingCSS,
	indicatorsContainer: indicatorsContainerCSS,
	indicatorSeparator: indicatorSeparatorCSS,
	input: inputCSS,
	loadingIndicator: loadingIndicatorCSS,
	loadingMessage: loadingMessageCSS,
	menu: menuCSS,
	menuList: menuListCSS,
	menuPortal: menuPortalCSS,
	multiValue: multiValueCSS,
	multiValueLabel: multiValueLabelCSS,
	multiValueRemove: multiValueRemoveCSS,
	noOptionsMessage: noOptionsMessageCSS,
	option: optionCSS,
	placeholder: placeholderCSS,
	singleValue: css,
	valueContainer: valueContainerCSS
};
var colors = {
	primary: "#2684FF",
	primary75: "#4C9AFF",
	primary50: "#B2D4FF",
	primary25: "#DEEBFF",
	danger: "#DE350B",
	dangerLight: "#FFBDAD",
	neutral0: "hsl(0, 0%, 100%)",
	neutral5: "hsl(0, 0%, 95%)",
	neutral10: "hsl(0, 0%, 90%)",
	neutral20: "hsl(0, 0%, 80%)",
	neutral30: "hsl(0, 0%, 70%)",
	neutral40: "hsl(0, 0%, 60%)",
	neutral50: "hsl(0, 0%, 50%)",
	neutral60: "hsl(0, 0%, 40%)",
	neutral70: "hsl(0, 0%, 30%)",
	neutral80: "hsl(0, 0%, 20%)",
	neutral90: "hsl(0, 0%, 10%)"
};
var borderRadius = 4;
var baseUnit = 4;
var defaultTheme = {
	borderRadius,
	colors,
	spacing: {
		baseUnit,
		controlHeight: 38,
		menuGutter: baseUnit * 2
	}
};
var defaultProps = {
	"aria-live": "polite",
	backspaceRemovesValue: true,
	blurInputOnSelect: isTouchCapable(),
	captureMenuScroll: !isTouchCapable(),
	classNames: {},
	closeMenuOnSelect: true,
	closeMenuOnScroll: false,
	components: {},
	controlShouldRenderValue: true,
	escapeClearsValue: false,
	filterOption: createFilter(),
	formatGroupLabel,
	getOptionLabel: getOptionLabel$1,
	getOptionValue: getOptionValue$1,
	isDisabled: false,
	isLoading: false,
	isMulti: false,
	isRtl: false,
	isSearchable: true,
	isOptionDisabled,
	loadingMessage: function loadingMessage() {
		return "Loading...";
	},
	maxMenuHeight: 300,
	minMenuHeight: 140,
	menuIsOpen: false,
	menuPlacement: "bottom",
	menuPosition: "absolute",
	menuShouldBlockScroll: false,
	menuShouldScrollIntoView: !isMobileDevice(),
	noOptionsMessage: function noOptionsMessage() {
		return "No options";
	},
	openMenuOnFocus: false,
	openMenuOnClick: true,
	options: [],
	pageSize: 5,
	placeholder: "Select...",
	screenReaderStatus: function screenReaderStatus(_ref) {
		var count = _ref.count;
		return "".concat(count, " result").concat(count !== 1 ? "s" : "", " available");
	},
	styles: {},
	tabIndex: 0,
	tabSelectsValue: true,
	unstyled: false
};
function toCategorizedOption(props, option, selectValue, index) {
	return {
		type: "option",
		data: option,
		isDisabled: _isOptionDisabled(props, option, selectValue),
		isSelected: _isOptionSelected(props, option, selectValue),
		label: getOptionLabel(props, option),
		value: getOptionValue(props, option),
		index
	};
}
function buildCategorizedOptions(props, selectValue) {
	return props.options.map(function(groupOrOption, groupOrOptionIndex) {
		if ("options" in groupOrOption) {
			var categorizedOptions = groupOrOption.options.map(function(option, optionIndex) {
				return toCategorizedOption(props, option, selectValue, optionIndex);
			}).filter(function(categorizedOption) {
				return isFocusable(props, categorizedOption);
			});
			return categorizedOptions.length > 0 ? {
				type: "group",
				data: groupOrOption,
				options: categorizedOptions,
				index: groupOrOptionIndex
			} : void 0;
		}
		var categorizedOption = toCategorizedOption(props, groupOrOption, selectValue, groupOrOptionIndex);
		return isFocusable(props, categorizedOption) ? categorizedOption : void 0;
	}).filter(notNullish);
}
function buildFocusableOptionsFromCategorizedOptions(categorizedOptions) {
	return categorizedOptions.reduce(function(optionsAccumulator, categorizedOption) {
		if (categorizedOption.type === "group") optionsAccumulator.push.apply(optionsAccumulator, _toConsumableArray(categorizedOption.options.map(function(option) {
			return option.data;
		})));
		else optionsAccumulator.push(categorizedOption.data);
		return optionsAccumulator;
	}, []);
}
function buildFocusableOptionsWithIds(categorizedOptions, optionId) {
	return categorizedOptions.reduce(function(optionsAccumulator, categorizedOption) {
		if (categorizedOption.type === "group") optionsAccumulator.push.apply(optionsAccumulator, _toConsumableArray(categorizedOption.options.map(function(option) {
			return {
				data: option.data,
				id: "".concat(optionId, "-").concat(categorizedOption.index, "-").concat(option.index)
			};
		})));
		else optionsAccumulator.push({
			data: categorizedOption.data,
			id: "".concat(optionId, "-").concat(categorizedOption.index)
		});
		return optionsAccumulator;
	}, []);
}
function buildFocusableOptions(props, selectValue) {
	return buildFocusableOptionsFromCategorizedOptions(buildCategorizedOptions(props, selectValue));
}
function isFocusable(props, categorizedOption) {
	var _props$inputValue = props.inputValue, inputValue = _props$inputValue === void 0 ? "" : _props$inputValue;
	var data = categorizedOption.data, isSelected = categorizedOption.isSelected, label = categorizedOption.label, value = categorizedOption.value;
	return (!shouldHideSelectedOptions(props) || !isSelected) && _filterOption(props, {
		label,
		value,
		data
	}, inputValue);
}
function getNextFocusedValue(state, nextSelectValue) {
	var focusedValue = state.focusedValue;
	var lastFocusedIndex = state.selectValue.indexOf(focusedValue);
	if (lastFocusedIndex > -1) {
		if (nextSelectValue.indexOf(focusedValue) > -1) return focusedValue;
		else if (lastFocusedIndex < nextSelectValue.length) return nextSelectValue[lastFocusedIndex];
	}
	return null;
}
function getNextFocusedOption(state, options) {
	var lastFocusedOption = state.focusedOption;
	return lastFocusedOption && options.indexOf(lastFocusedOption) > -1 ? lastFocusedOption : options[0];
}
var getFocusedOptionId = function getFocusedOptionId(focusableOptionsWithIds, focusedOption) {
	var _focusableOptionsWith;
	return ((_focusableOptionsWith = focusableOptionsWithIds.find(function(option) {
		return option.data === focusedOption;
	})) === null || _focusableOptionsWith === void 0 ? void 0 : _focusableOptionsWith.id) || null;
};
var getOptionLabel = function getOptionLabel(props, data) {
	return props.getOptionLabel(data);
};
var getOptionValue = function getOptionValue(props, data) {
	return props.getOptionValue(data);
};
function _isOptionDisabled(props, option, selectValue) {
	return typeof props.isOptionDisabled === "function" ? props.isOptionDisabled(option, selectValue) : false;
}
function _isOptionSelected(props, option, selectValue) {
	if (selectValue.indexOf(option) > -1) return true;
	if (typeof props.isOptionSelected === "function") return props.isOptionSelected(option, selectValue);
	var candidate = getOptionValue(props, option);
	return selectValue.some(function(i) {
		return getOptionValue(props, i) === candidate;
	});
}
function _filterOption(props, option, inputValue) {
	return props.filterOption ? props.filterOption(option, inputValue) : true;
}
var shouldHideSelectedOptions = function shouldHideSelectedOptions(props) {
	var hideSelectedOptions = props.hideSelectedOptions, isMulti = props.isMulti;
	if (hideSelectedOptions === void 0) return isMulti;
	return hideSelectedOptions;
};
var instanceId = 1;
var Select = /*#__PURE__*/ function(_Component) {
	_inherits(Select, _Component);
	var _super = _createSuper(Select);
	function Select(_props) {
		var _this;
		_classCallCheck(this, Select);
		_this = _super.call(this, _props);
		_this.state = {
			ariaSelection: null,
			focusedOption: null,
			focusedOptionId: null,
			focusableOptionsWithIds: [],
			focusedValue: null,
			inputIsHidden: false,
			isFocused: false,
			selectValue: [],
			clearFocusValueOnUpdate: false,
			prevWasFocused: false,
			inputIsHiddenAfterUpdate: void 0,
			prevProps: void 0,
			instancePrefix: "",
			isAppleDevice: false
		};
		_this.blockOptionHover = false;
		_this.isComposing = false;
		_this.commonProps = void 0;
		_this.initialTouchX = 0;
		_this.initialTouchY = 0;
		_this.openAfterFocus = false;
		_this.scrollToFocusedOptionOnUpdate = false;
		_this.userIsDragging = void 0;
		_this.controlRef = null;
		_this.getControlRef = function(ref) {
			_this.controlRef = ref;
		};
		_this.focusedOptionRef = null;
		_this.getFocusedOptionRef = function(ref) {
			_this.focusedOptionRef = ref;
		};
		_this.menuListRef = null;
		_this.getMenuListRef = function(ref) {
			_this.menuListRef = ref;
		};
		_this.inputRef = null;
		_this.getInputRef = function(ref) {
			_this.inputRef = ref;
		};
		_this.focus = _this.focusInput;
		_this.blur = _this.blurInput;
		_this.onChange = function(newValue, actionMeta) {
			var _this$props = _this.props, onChange = _this$props.onChange;
			actionMeta.name = _this$props.name;
			_this.ariaOnChange(newValue, actionMeta);
			onChange(newValue, actionMeta);
		};
		_this.setValue = function(newValue, action, option) {
			var _this$props2 = _this.props, closeMenuOnSelect = _this$props2.closeMenuOnSelect, isMulti = _this$props2.isMulti, inputValue = _this$props2.inputValue;
			_this.onInputChange("", {
				action: "set-value",
				prevInputValue: inputValue
			});
			if (closeMenuOnSelect) {
				_this.setState({ inputIsHiddenAfterUpdate: !isMulti });
				_this.onMenuClose();
			}
			_this.setState({ clearFocusValueOnUpdate: true });
			_this.onChange(newValue, {
				action,
				option
			});
		};
		_this.selectOption = function(newValue) {
			var _this$props3 = _this.props, blurInputOnSelect = _this$props3.blurInputOnSelect, isMulti = _this$props3.isMulti, name = _this$props3.name;
			var selectValue = _this.state.selectValue;
			var deselected = isMulti && _this.isOptionSelected(newValue, selectValue);
			var isDisabled = _this.isOptionDisabled(newValue, selectValue);
			if (deselected) {
				var candidate = _this.getOptionValue(newValue);
				_this.setValue(multiValueAsValue(selectValue.filter(function(i) {
					return _this.getOptionValue(i) !== candidate;
				})), "deselect-option", newValue);
			} else if (!isDisabled) if (isMulti) _this.setValue(multiValueAsValue([].concat(_toConsumableArray(selectValue), [newValue])), "select-option", newValue);
			else _this.setValue(singleValueAsValue(newValue), "select-option");
			else {
				_this.ariaOnChange(singleValueAsValue(newValue), {
					action: "select-option",
					option: newValue,
					name
				});
				return;
			}
			if (blurInputOnSelect) _this.blurInput();
		};
		_this.removeValue = function(removedValue) {
			var isMulti = _this.props.isMulti;
			var selectValue = _this.state.selectValue;
			var candidate = _this.getOptionValue(removedValue);
			var newValueArray = selectValue.filter(function(i) {
				return _this.getOptionValue(i) !== candidate;
			});
			var newValue = valueTernary(isMulti, newValueArray, newValueArray[0] || null);
			_this.onChange(newValue, {
				action: "remove-value",
				removedValue
			});
			_this.focusInput();
		};
		_this.clearValue = function() {
			var selectValue = _this.state.selectValue;
			_this.onChange(valueTernary(_this.props.isMulti, [], null), {
				action: "clear",
				removedValues: selectValue
			});
		};
		_this.popValue = function() {
			var isMulti = _this.props.isMulti;
			var selectValue = _this.state.selectValue;
			var lastSelectedValue = selectValue[selectValue.length - 1];
			var newValueArray = selectValue.slice(0, selectValue.length - 1);
			var newValue = valueTernary(isMulti, newValueArray, newValueArray[0] || null);
			if (lastSelectedValue) _this.onChange(newValue, {
				action: "pop-value",
				removedValue: lastSelectedValue
			});
		};
		_this.getFocusedOptionId = function(focusedOption) {
			return getFocusedOptionId(_this.state.focusableOptionsWithIds, focusedOption);
		};
		_this.getFocusableOptionsWithIds = function() {
			return buildFocusableOptionsWithIds(buildCategorizedOptions(_this.props, _this.state.selectValue), _this.getElementId("option"));
		};
		_this.getValue = function() {
			return _this.state.selectValue;
		};
		_this.cx = function() {
			for (var _len = arguments.length, args = new Array(_len), _key = 0; _key < _len; _key++) args[_key] = arguments[_key];
			return classNames.apply(void 0, [_this.props.classNamePrefix].concat(args));
		};
		_this.getOptionLabel = function(data) {
			return getOptionLabel(_this.props, data);
		};
		_this.getOptionValue = function(data) {
			return getOptionValue(_this.props, data);
		};
		_this.getStyles = function(key, props) {
			var unstyled = _this.props.unstyled;
			var base = defaultStyles[key](props, unstyled);
			base.boxSizing = "border-box";
			var custom = _this.props.styles[key];
			return custom ? custom(base, props) : base;
		};
		_this.getClassNames = function(key, props) {
			var _this$props$className, _this$props$className2;
			return (_this$props$className = (_this$props$className2 = _this.props.classNames)[key]) === null || _this$props$className === void 0 ? void 0 : _this$props$className.call(_this$props$className2, props);
		};
		_this.getElementId = function(element) {
			return "".concat(_this.state.instancePrefix, "-").concat(element);
		};
		_this.getComponents = function() {
			return defaultComponents(_this.props);
		};
		_this.buildCategorizedOptions = function() {
			return buildCategorizedOptions(_this.props, _this.state.selectValue);
		};
		_this.getCategorizedOptions = function() {
			return _this.props.menuIsOpen ? _this.buildCategorizedOptions() : [];
		};
		_this.buildFocusableOptions = function() {
			return buildFocusableOptionsFromCategorizedOptions(_this.buildCategorizedOptions());
		};
		_this.getFocusableOptions = function() {
			return _this.props.menuIsOpen ? _this.buildFocusableOptions() : [];
		};
		_this.ariaOnChange = function(value, actionMeta) {
			_this.setState({ ariaSelection: _objectSpread2({ value }, actionMeta) });
		};
		_this.onMenuMouseDown = function(event) {
			if (event.button !== 0) return;
			event.stopPropagation();
			event.preventDefault();
			_this.focusInput();
		};
		_this.onMenuMouseMove = function(event) {
			_this.blockOptionHover = false;
		};
		_this.onControlMouseDown = function(event) {
			if (event.defaultPrevented) return;
			var openMenuOnClick = _this.props.openMenuOnClick;
			if (!_this.state.isFocused) {
				if (openMenuOnClick) _this.openAfterFocus = true;
				_this.focusInput();
			} else if (!_this.props.menuIsOpen) {
				if (openMenuOnClick) _this.openMenu("first");
			} else if (event.target.tagName !== "INPUT" && event.target.tagName !== "TEXTAREA") _this.onMenuClose();
			if (event.target.tagName !== "INPUT" && event.target.tagName !== "TEXTAREA") event.preventDefault();
		};
		_this.onDropdownIndicatorMouseDown = function(event) {
			if (event && event.type === "mousedown" && event.button !== 0) return;
			if (_this.props.isDisabled) return;
			var _this$props4 = _this.props, isMulti = _this$props4.isMulti, menuIsOpen = _this$props4.menuIsOpen;
			_this.focusInput();
			if (menuIsOpen) {
				_this.setState({ inputIsHiddenAfterUpdate: !isMulti });
				_this.onMenuClose();
			} else _this.openMenu("first");
			event.preventDefault();
		};
		_this.onClearIndicatorMouseDown = function(event) {
			if (event && event.type === "mousedown" && event.button !== 0) return;
			_this.clearValue();
			event.preventDefault();
			_this.openAfterFocus = false;
			if (event.type === "touchend") _this.focusInput();
			else setTimeout(function() {
				return _this.focusInput();
			});
		};
		_this.onScroll = function(event) {
			if (typeof _this.props.closeMenuOnScroll === "boolean") {
				if (event.target instanceof HTMLElement && isDocumentElement(event.target)) _this.props.onMenuClose();
			} else if (typeof _this.props.closeMenuOnScroll === "function") {
				if (_this.props.closeMenuOnScroll(event)) _this.props.onMenuClose();
			}
		};
		_this.onCompositionStart = function() {
			_this.isComposing = true;
		};
		_this.onCompositionEnd = function() {
			_this.isComposing = false;
		};
		_this.onTouchStart = function(_ref2) {
			var touches = _ref2.touches;
			var touch = touches && touches.item(0);
			if (!touch) return;
			_this.initialTouchX = touch.clientX;
			_this.initialTouchY = touch.clientY;
			_this.userIsDragging = false;
		};
		_this.onTouchMove = function(_ref3) {
			var touches = _ref3.touches;
			var touch = touches && touches.item(0);
			if (!touch) return;
			var deltaX = Math.abs(touch.clientX - _this.initialTouchX);
			var deltaY = Math.abs(touch.clientY - _this.initialTouchY);
			var moveThreshold = 5;
			_this.userIsDragging = deltaX > moveThreshold || deltaY > moveThreshold;
		};
		_this.onTouchEnd = function(event) {
			if (_this.userIsDragging) return;
			if (_this.controlRef && !_this.controlRef.contains(event.target) && _this.menuListRef && !_this.menuListRef.contains(event.target)) _this.blurInput();
			_this.initialTouchX = 0;
			_this.initialTouchY = 0;
		};
		_this.onControlTouchEnd = function(event) {
			if (_this.userIsDragging) return;
			_this.onControlMouseDown(event);
		};
		_this.onClearIndicatorTouchEnd = function(event) {
			if (_this.userIsDragging) return;
			_this.onClearIndicatorMouseDown(event);
		};
		_this.onDropdownIndicatorTouchEnd = function(event) {
			if (_this.userIsDragging) return;
			_this.onDropdownIndicatorMouseDown(event);
		};
		_this.handleInputChange = function(event) {
			var prevInputValue = _this.props.inputValue;
			var inputValue = event.currentTarget.value;
			_this.setState({ inputIsHiddenAfterUpdate: false });
			_this.onInputChange(inputValue, {
				action: "input-change",
				prevInputValue
			});
			if (!_this.props.menuIsOpen) _this.onMenuOpen();
		};
		_this.onInputFocus = function(event) {
			if (_this.props.onFocus) _this.props.onFocus(event);
			_this.setState({
				inputIsHiddenAfterUpdate: false,
				isFocused: true
			});
			if (_this.openAfterFocus || _this.props.openMenuOnFocus) _this.openMenu("first");
			_this.openAfterFocus = false;
		};
		_this.onInputBlur = function(event) {
			var prevInputValue = _this.props.inputValue;
			if (_this.menuListRef && _this.menuListRef.contains(document.activeElement)) {
				_this.inputRef.focus();
				return;
			}
			if (_this.props.onBlur) _this.props.onBlur(event);
			_this.onInputChange("", {
				action: "input-blur",
				prevInputValue
			});
			_this.onMenuClose();
			_this.setState({
				focusedValue: null,
				isFocused: false
			});
		};
		_this.onOptionHover = function(focusedOption) {
			if (_this.blockOptionHover || _this.state.focusedOption === focusedOption) return;
			var focusedOptionIndex = _this.getFocusableOptions().indexOf(focusedOption);
			_this.setState({
				focusedOption,
				focusedOptionId: focusedOptionIndex > -1 ? _this.getFocusedOptionId(focusedOption) : null
			});
		};
		_this.shouldHideSelectedOptions = function() {
			return shouldHideSelectedOptions(_this.props);
		};
		_this.onValueInputFocus = function(e) {
			e.preventDefault();
			e.stopPropagation();
			_this.focus();
		};
		_this.onKeyDown = function(event) {
			var _this$props5 = _this.props, isMulti = _this$props5.isMulti, backspaceRemovesValue = _this$props5.backspaceRemovesValue, escapeClearsValue = _this$props5.escapeClearsValue, inputValue = _this$props5.inputValue, isClearable = _this$props5.isClearable, isDisabled = _this$props5.isDisabled, menuIsOpen = _this$props5.menuIsOpen, onKeyDown = _this$props5.onKeyDown, tabSelectsValue = _this$props5.tabSelectsValue, openMenuOnFocus = _this$props5.openMenuOnFocus;
			var _this$state = _this.state, focusedOption = _this$state.focusedOption, focusedValue = _this$state.focusedValue, selectValue = _this$state.selectValue;
			if (isDisabled) return;
			if (typeof onKeyDown === "function") {
				onKeyDown(event);
				if (event.defaultPrevented) return;
			}
			_this.blockOptionHover = true;
			switch (event.key) {
				case "ArrowLeft":
					if (!isMulti || inputValue) return;
					_this.focusValue("previous");
					break;
				case "ArrowRight":
					if (!isMulti || inputValue) return;
					_this.focusValue("next");
					break;
				case "Delete":
				case "Backspace":
					if (inputValue) return;
					if (focusedValue) _this.removeValue(focusedValue);
					else {
						if (!backspaceRemovesValue) return;
						if (isMulti) _this.popValue();
						else if (isClearable) _this.clearValue();
					}
					break;
				case "Tab":
					if (_this.isComposing) return;
					if (event.shiftKey || !menuIsOpen || !tabSelectsValue || !focusedOption || openMenuOnFocus && _this.isOptionSelected(focusedOption, selectValue)) return;
					_this.selectOption(focusedOption);
					break;
				case "Enter":
					if (event.keyCode === 229) break;
					if (menuIsOpen) {
						if (!focusedOption) return;
						if (_this.isComposing) return;
						_this.selectOption(focusedOption);
						break;
					}
					return;
				case "Escape":
					if (menuIsOpen) {
						_this.setState({ inputIsHiddenAfterUpdate: false });
						_this.onInputChange("", {
							action: "menu-close",
							prevInputValue: inputValue
						});
						_this.onMenuClose();
					} else if (isClearable && escapeClearsValue) _this.clearValue();
					break;
				case " ":
					if (inputValue) return;
					if (!menuIsOpen) {
						_this.openMenu("first");
						break;
					}
					if (!focusedOption) return;
					_this.selectOption(focusedOption);
					break;
				case "ArrowUp":
					if (menuIsOpen) _this.focusOption("up");
					else _this.openMenu("last");
					break;
				case "ArrowDown":
					if (menuIsOpen) _this.focusOption("down");
					else _this.openMenu("first");
					break;
				case "PageUp":
					if (!menuIsOpen) return;
					_this.focusOption("pageup");
					break;
				case "PageDown":
					if (!menuIsOpen) return;
					_this.focusOption("pagedown");
					break;
				case "Home":
					if (!menuIsOpen) return;
					_this.focusOption("first");
					break;
				case "End":
					if (!menuIsOpen) return;
					_this.focusOption("last");
					break;
				default: return;
			}
			event.preventDefault();
		};
		_this.state.instancePrefix = "react-select-" + (_this.props.instanceId || ++instanceId);
		_this.state.selectValue = cleanValue(_props.value);
		if (_props.menuIsOpen && _this.state.selectValue.length) {
			var focusableOptionsWithIds = _this.getFocusableOptionsWithIds();
			var focusableOptions = _this.buildFocusableOptions();
			var optionIndex = focusableOptions.indexOf(_this.state.selectValue[0]);
			_this.state.focusableOptionsWithIds = focusableOptionsWithIds;
			_this.state.focusedOption = focusableOptions[optionIndex];
			_this.state.focusedOptionId = getFocusedOptionId(focusableOptionsWithIds, focusableOptions[optionIndex]);
		}
		return _this;
	}
	_createClass(Select, [
		{
			key: "componentDidMount",
			value: function componentDidMount() {
				this.startListeningComposition();
				this.startListeningToTouch();
				if (this.props.closeMenuOnScroll && document && document.addEventListener) document.addEventListener("scroll", this.onScroll, true);
				if (this.props.autoFocus) this.focusInput();
				if (this.props.menuIsOpen && this.state.focusedOption && this.menuListRef && this.focusedOptionRef) scrollIntoView(this.menuListRef, this.focusedOptionRef);
				if (isAppleDevice()) this.setState({ isAppleDevice: true });
			}
		},
		{
			key: "componentDidUpdate",
			value: function componentDidUpdate(prevProps) {
				var _this$props6 = this.props, isDisabled = _this$props6.isDisabled, menuIsOpen = _this$props6.menuIsOpen;
				var isFocused = this.state.isFocused;
				if (isFocused && !isDisabled && prevProps.isDisabled || isFocused && menuIsOpen && !prevProps.menuIsOpen) this.focusInput();
				if (isFocused && isDisabled && !prevProps.isDisabled) this.setState({ isFocused: false }, this.onMenuClose);
				else if (!isFocused && !isDisabled && prevProps.isDisabled && this.inputRef === document.activeElement) this.setState({ isFocused: true });
				if (this.menuListRef && this.focusedOptionRef && this.scrollToFocusedOptionOnUpdate) {
					scrollIntoView(this.menuListRef, this.focusedOptionRef);
					this.scrollToFocusedOptionOnUpdate = false;
				}
			}
		},
		{
			key: "componentWillUnmount",
			value: function componentWillUnmount() {
				this.stopListeningComposition();
				this.stopListeningToTouch();
				document.removeEventListener("scroll", this.onScroll, true);
			}
		},
		{
			key: "onMenuOpen",
			value: function onMenuOpen() {
				this.props.onMenuOpen();
			}
		},
		{
			key: "onMenuClose",
			value: function onMenuClose() {
				this.onInputChange("", {
					action: "menu-close",
					prevInputValue: this.props.inputValue
				});
				this.props.onMenuClose();
			}
		},
		{
			key: "onInputChange",
			value: function onInputChange(newValue, actionMeta) {
				this.props.onInputChange(newValue, actionMeta);
			}
		},
		{
			key: "focusInput",
			value: function focusInput() {
				if (!this.inputRef) return;
				this.inputRef.focus();
			}
		},
		{
			key: "blurInput",
			value: function blurInput() {
				if (!this.inputRef) return;
				this.inputRef.blur();
			}
		},
		{
			key: "openMenu",
			value: function openMenu(focusOption) {
				var _this2 = this;
				var _this$state2 = this.state, selectValue = _this$state2.selectValue, isFocused = _this$state2.isFocused;
				var focusableOptions = this.buildFocusableOptions();
				var openAtIndex = focusOption === "first" ? 0 : focusableOptions.length - 1;
				if (!this.props.isMulti) {
					var selectedIndex = focusableOptions.indexOf(selectValue[0]);
					if (selectedIndex > -1) openAtIndex = selectedIndex;
				}
				this.scrollToFocusedOptionOnUpdate = !(isFocused && this.menuListRef);
				this.setState({
					inputIsHiddenAfterUpdate: false,
					focusedValue: null,
					focusedOption: focusableOptions[openAtIndex],
					focusedOptionId: this.getFocusedOptionId(focusableOptions[openAtIndex])
				}, function() {
					return _this2.onMenuOpen();
				});
			}
		},
		{
			key: "focusValue",
			value: function focusValue(direction) {
				var _this$state3 = this.state, selectValue = _this$state3.selectValue, focusedValue = _this$state3.focusedValue;
				if (!this.props.isMulti) return;
				this.setState({ focusedOption: null });
				var focusedIndex = selectValue.indexOf(focusedValue);
				if (!focusedValue) focusedIndex = -1;
				var lastIndex = selectValue.length - 1;
				var nextFocus = -1;
				if (!selectValue.length) return;
				switch (direction) {
					case "previous":
						if (focusedIndex === 0) nextFocus = 0;
						else if (focusedIndex === -1) nextFocus = lastIndex;
						else nextFocus = focusedIndex - 1;
						break;
					case "next":
						if (focusedIndex > -1 && focusedIndex < lastIndex) nextFocus = focusedIndex + 1;
						break;
				}
				this.setState({
					inputIsHidden: nextFocus !== -1,
					focusedValue: selectValue[nextFocus]
				});
			}
		},
		{
			key: "focusOption",
			value: function focusOption() {
				var direction = arguments.length > 0 && arguments[0] !== void 0 ? arguments[0] : "first";
				var pageSize = this.props.pageSize;
				var focusedOption = this.state.focusedOption;
				var options = this.getFocusableOptions();
				if (!options.length) return;
				var nextFocus = 0;
				var focusedIndex = options.indexOf(focusedOption);
				if (!focusedOption) focusedIndex = -1;
				if (direction === "up") nextFocus = focusedIndex > 0 ? focusedIndex - 1 : options.length - 1;
				else if (direction === "down") nextFocus = (focusedIndex + 1) % options.length;
				else if (direction === "pageup") {
					nextFocus = focusedIndex - pageSize;
					if (nextFocus < 0) nextFocus = 0;
				} else if (direction === "pagedown") {
					nextFocus = focusedIndex + pageSize;
					if (nextFocus > options.length - 1) nextFocus = options.length - 1;
				} else if (direction === "last") nextFocus = options.length - 1;
				this.scrollToFocusedOptionOnUpdate = true;
				this.setState({
					focusedOption: options[nextFocus],
					focusedValue: null,
					focusedOptionId: this.getFocusedOptionId(options[nextFocus])
				});
			}
		},
		{
			key: "getTheme",
			value: function getTheme() {
				if (!this.props.theme) return defaultTheme;
				if (typeof this.props.theme === "function") return this.props.theme(defaultTheme);
				return _objectSpread2(_objectSpread2({}, defaultTheme), this.props.theme);
			}
		},
		{
			key: "getCommonProps",
			value: function getCommonProps() {
				var clearValue = this.clearValue, cx = this.cx, getStyles = this.getStyles, getClassNames = this.getClassNames, getValue = this.getValue, selectOption = this.selectOption, setValue = this.setValue, props = this.props;
				var isMulti = props.isMulti, isRtl = props.isRtl, options = props.options;
				return {
					clearValue,
					cx,
					getStyles,
					getClassNames,
					getValue,
					hasValue: this.hasValue(),
					isMulti,
					isRtl,
					options,
					selectOption,
					selectProps: props,
					setValue,
					theme: this.getTheme()
				};
			}
		},
		{
			key: "hasValue",
			value: function hasValue() {
				return this.state.selectValue.length > 0;
			}
		},
		{
			key: "hasOptions",
			value: function hasOptions() {
				return !!this.getFocusableOptions().length;
			}
		},
		{
			key: "isClearable",
			value: function isClearable() {
				var _this$props7 = this.props, isClearable = _this$props7.isClearable, isMulti = _this$props7.isMulti;
				if (isClearable === void 0) return isMulti;
				return isClearable;
			}
		},
		{
			key: "isOptionDisabled",
			value: function isOptionDisabled(option, selectValue) {
				return _isOptionDisabled(this.props, option, selectValue);
			}
		},
		{
			key: "isOptionSelected",
			value: function isOptionSelected(option, selectValue) {
				return _isOptionSelected(this.props, option, selectValue);
			}
		},
		{
			key: "filterOption",
			value: function filterOption(option, inputValue) {
				return _filterOption(this.props, option, inputValue);
			}
		},
		{
			key: "formatOptionLabel",
			value: function formatOptionLabel(data, context) {
				if (typeof this.props.formatOptionLabel === "function") {
					var _inputValue = this.props.inputValue;
					var _selectValue = this.state.selectValue;
					return this.props.formatOptionLabel(data, {
						context,
						inputValue: _inputValue,
						selectValue: _selectValue
					});
				} else return this.getOptionLabel(data);
			}
		},
		{
			key: "formatGroupLabel",
			value: function formatGroupLabel(data) {
				return this.props.formatGroupLabel(data);
			}
		},
		{
			key: "startListeningComposition",
			value: function startListeningComposition() {
				if (document && document.addEventListener) {
					document.addEventListener("compositionstart", this.onCompositionStart, false);
					document.addEventListener("compositionend", this.onCompositionEnd, false);
				}
			}
		},
		{
			key: "stopListeningComposition",
			value: function stopListeningComposition() {
				if (document && document.removeEventListener) {
					document.removeEventListener("compositionstart", this.onCompositionStart);
					document.removeEventListener("compositionend", this.onCompositionEnd);
				}
			}
		},
		{
			key: "startListeningToTouch",
			value: function startListeningToTouch() {
				if (document && document.addEventListener) {
					document.addEventListener("touchstart", this.onTouchStart, false);
					document.addEventListener("touchmove", this.onTouchMove, false);
					document.addEventListener("touchend", this.onTouchEnd, false);
				}
			}
		},
		{
			key: "stopListeningToTouch",
			value: function stopListeningToTouch() {
				if (document && document.removeEventListener) {
					document.removeEventListener("touchstart", this.onTouchStart);
					document.removeEventListener("touchmove", this.onTouchMove);
					document.removeEventListener("touchend", this.onTouchEnd);
				}
			}
		},
		{
			key: "renderInput",
			value: function renderInput() {
				var _this$props8 = this.props, isDisabled = _this$props8.isDisabled, isSearchable = _this$props8.isSearchable, inputId = _this$props8.inputId, inputValue = _this$props8.inputValue, tabIndex = _this$props8.tabIndex, form = _this$props8.form, menuIsOpen = _this$props8.menuIsOpen, required = _this$props8.required;
				var Input = this.getComponents().Input;
				var _this$state4 = this.state, inputIsHidden = _this$state4.inputIsHidden, ariaSelection = _this$state4.ariaSelection;
				var commonProps = this.commonProps;
				var id = inputId || this.getElementId("input");
				var ariaAttributes = _objectSpread2(_objectSpread2(_objectSpread2({
					"aria-autocomplete": "list",
					"aria-expanded": menuIsOpen,
					"aria-haspopup": true,
					"aria-errormessage": this.props["aria-errormessage"],
					"aria-invalid": this.props["aria-invalid"],
					"aria-label": this.props["aria-label"],
					"aria-labelledby": this.props["aria-labelledby"],
					"aria-required": required,
					role: "combobox",
					"aria-activedescendant": this.state.isAppleDevice ? void 0 : this.state.focusedOptionId || ""
				}, menuIsOpen && { "aria-controls": this.getElementId("listbox") }), !isSearchable && { "aria-readonly": true }), this.hasValue() ? (ariaSelection === null || ariaSelection === void 0 ? void 0 : ariaSelection.action) === "initial-input-focus" && { "aria-describedby": this.getElementId("live-region") } : { "aria-describedby": this.getElementId("placeholder") });
				if (!isSearchable) return /*#__PURE__*/ import_react.createElement(DummyInput, _extends({
					id,
					innerRef: this.getInputRef,
					onBlur: this.onInputBlur,
					onChange: noop,
					onFocus: this.onInputFocus,
					disabled: isDisabled,
					tabIndex,
					inputMode: "none",
					form,
					value: ""
				}, ariaAttributes));
				return /*#__PURE__*/ import_react.createElement(Input, _extends({}, commonProps, {
					autoCapitalize: "none",
					autoComplete: "off",
					autoCorrect: "off",
					id,
					innerRef: this.getInputRef,
					isDisabled,
					isHidden: inputIsHidden,
					onBlur: this.onInputBlur,
					onChange: this.handleInputChange,
					onFocus: this.onInputFocus,
					spellCheck: "false",
					tabIndex,
					form,
					type: "text",
					value: inputValue
				}, ariaAttributes));
			}
		},
		{
			key: "renderPlaceholderOrValue",
			value: function renderPlaceholderOrValue() {
				var _this3 = this;
				var _this$getComponents2 = this.getComponents(), MultiValue = _this$getComponents2.MultiValue, MultiValueContainer = _this$getComponents2.MultiValueContainer, MultiValueLabel = _this$getComponents2.MultiValueLabel, MultiValueRemove = _this$getComponents2.MultiValueRemove, SingleValue = _this$getComponents2.SingleValue, Placeholder = _this$getComponents2.Placeholder;
				var commonProps = this.commonProps;
				var _this$props9 = this.props, controlShouldRenderValue = _this$props9.controlShouldRenderValue, isDisabled = _this$props9.isDisabled, isMulti = _this$props9.isMulti, inputValue = _this$props9.inputValue, placeholder = _this$props9.placeholder;
				var _this$state5 = this.state, selectValue = _this$state5.selectValue, focusedValue = _this$state5.focusedValue, isFocused = _this$state5.isFocused;
				if (!this.hasValue() || !controlShouldRenderValue) return inputValue ? null : /*#__PURE__*/ import_react.createElement(Placeholder, _extends({}, commonProps, {
					key: "placeholder",
					isDisabled,
					isFocused,
					innerProps: { id: this.getElementId("placeholder") }
				}), placeholder);
				if (isMulti) return selectValue.map(function(opt, index) {
					var isOptionFocused = opt === focusedValue;
					var key = "".concat(_this3.getOptionLabel(opt), "-").concat(_this3.getOptionValue(opt));
					return /*#__PURE__*/ import_react.createElement(MultiValue, _extends({}, commonProps, {
						components: {
							Container: MultiValueContainer,
							Label: MultiValueLabel,
							Remove: MultiValueRemove
						},
						isFocused: isOptionFocused,
						isDisabled,
						key,
						index,
						removeProps: {
							onClick: function onClick() {
								return _this3.removeValue(opt);
							},
							onTouchEnd: function onTouchEnd() {
								return _this3.removeValue(opt);
							},
							onMouseDown: function onMouseDown(e) {
								e.preventDefault();
							}
						},
						data: opt
					}), _this3.formatOptionLabel(opt, "value"));
				});
				if (inputValue) return null;
				var singleValue = selectValue[0];
				return /*#__PURE__*/ import_react.createElement(SingleValue, _extends({}, commonProps, {
					data: singleValue,
					isDisabled
				}), this.formatOptionLabel(singleValue, "value"));
			}
		},
		{
			key: "renderClearIndicator",
			value: function renderClearIndicator() {
				var ClearIndicator = this.getComponents().ClearIndicator;
				var commonProps = this.commonProps;
				var _this$props10 = this.props, isDisabled = _this$props10.isDisabled, isLoading = _this$props10.isLoading;
				var isFocused = this.state.isFocused;
				if (!this.isClearable() || !ClearIndicator || isDisabled || !this.hasValue() || isLoading) return null;
				var innerProps = {
					onMouseDown: this.onClearIndicatorMouseDown,
					onTouchEnd: this.onClearIndicatorTouchEnd,
					"aria-hidden": "true"
				};
				return /*#__PURE__*/ import_react.createElement(ClearIndicator, _extends({}, commonProps, {
					innerProps,
					isFocused
				}));
			}
		},
		{
			key: "renderLoadingIndicator",
			value: function renderLoadingIndicator() {
				var LoadingIndicator = this.getComponents().LoadingIndicator;
				var commonProps = this.commonProps;
				var _this$props11 = this.props, isDisabled = _this$props11.isDisabled, isLoading = _this$props11.isLoading;
				var isFocused = this.state.isFocused;
				if (!LoadingIndicator || !isLoading) return null;
				return /*#__PURE__*/ import_react.createElement(LoadingIndicator, _extends({}, commonProps, {
					innerProps: { "aria-hidden": "true" },
					isDisabled,
					isFocused
				}));
			}
		},
		{
			key: "renderIndicatorSeparator",
			value: function renderIndicatorSeparator() {
				var _this$getComponents5 = this.getComponents(), DropdownIndicator = _this$getComponents5.DropdownIndicator, IndicatorSeparator = _this$getComponents5.IndicatorSeparator;
				if (!DropdownIndicator || !IndicatorSeparator) return null;
				var commonProps = this.commonProps;
				var isDisabled = this.props.isDisabled;
				var isFocused = this.state.isFocused;
				return /*#__PURE__*/ import_react.createElement(IndicatorSeparator, _extends({}, commonProps, {
					isDisabled,
					isFocused
				}));
			}
		},
		{
			key: "renderDropdownIndicator",
			value: function renderDropdownIndicator() {
				var DropdownIndicator = this.getComponents().DropdownIndicator;
				if (!DropdownIndicator) return null;
				var commonProps = this.commonProps;
				var isDisabled = this.props.isDisabled;
				var isFocused = this.state.isFocused;
				var innerProps = {
					onMouseDown: this.onDropdownIndicatorMouseDown,
					onTouchEnd: this.onDropdownIndicatorTouchEnd,
					"aria-hidden": "true"
				};
				return /*#__PURE__*/ import_react.createElement(DropdownIndicator, _extends({}, commonProps, {
					innerProps,
					isDisabled,
					isFocused
				}));
			}
		},
		{
			key: "renderMenu",
			value: function renderMenu() {
				var _this4 = this;
				var _this$getComponents7 = this.getComponents(), Group = _this$getComponents7.Group, GroupHeading = _this$getComponents7.GroupHeading, Menu = _this$getComponents7.Menu, MenuList = _this$getComponents7.MenuList, MenuPortal = _this$getComponents7.MenuPortal, LoadingMessage = _this$getComponents7.LoadingMessage, NoOptionsMessage = _this$getComponents7.NoOptionsMessage, Option = _this$getComponents7.Option;
				var commonProps = this.commonProps;
				var focusedOption = this.state.focusedOption;
				var _this$props12 = this.props, captureMenuScroll = _this$props12.captureMenuScroll, inputValue = _this$props12.inputValue, isLoading = _this$props12.isLoading, loadingMessage = _this$props12.loadingMessage, minMenuHeight = _this$props12.minMenuHeight, maxMenuHeight = _this$props12.maxMenuHeight, menuIsOpen = _this$props12.menuIsOpen, menuPlacement = _this$props12.menuPlacement, menuPosition = _this$props12.menuPosition, menuPortalTarget = _this$props12.menuPortalTarget, menuShouldBlockScroll = _this$props12.menuShouldBlockScroll, menuShouldScrollIntoView = _this$props12.menuShouldScrollIntoView, noOptionsMessage = _this$props12.noOptionsMessage, onMenuScrollToTop = _this$props12.onMenuScrollToTop, onMenuScrollToBottom = _this$props12.onMenuScrollToBottom;
				if (!menuIsOpen) return null;
				var render = function render(props, id) {
					var type = props.type, data = props.data, isDisabled = props.isDisabled, isSelected = props.isSelected, label = props.label, value = props.value;
					var isFocused = focusedOption === data;
					var onHover = isDisabled ? void 0 : function() {
						return _this4.onOptionHover(data);
					};
					var onSelect = isDisabled ? void 0 : function() {
						return _this4.selectOption(data);
					};
					var optionId = "".concat(_this4.getElementId("option"), "-").concat(id);
					var innerProps = {
						id: optionId,
						onClick: onSelect,
						onMouseMove: onHover,
						onMouseOver: onHover,
						tabIndex: -1,
						role: "option",
						"aria-selected": _this4.state.isAppleDevice ? void 0 : isSelected
					};
					return /*#__PURE__*/ import_react.createElement(Option, _extends({}, commonProps, {
						innerProps,
						data,
						isDisabled,
						isSelected,
						key: optionId,
						label,
						type,
						value,
						isFocused,
						innerRef: isFocused ? _this4.getFocusedOptionRef : void 0
					}), _this4.formatOptionLabel(props.data, "menu"));
				};
				var menuUI;
				if (this.hasOptions()) menuUI = this.getCategorizedOptions().map(function(item) {
					if (item.type === "group") {
						var _data = item.data, options = item.options, groupIndex = item.index;
						var groupId = "".concat(_this4.getElementId("group"), "-").concat(groupIndex);
						var headingId = "".concat(groupId, "-heading");
						return /*#__PURE__*/ import_react.createElement(Group, _extends({}, commonProps, {
							key: groupId,
							data: _data,
							options,
							Heading: GroupHeading,
							headingProps: {
								id: headingId,
								data: item.data
							},
							label: _this4.formatGroupLabel(item.data)
						}), item.options.map(function(option) {
							return render(option, "".concat(groupIndex, "-").concat(option.index));
						}));
					} else if (item.type === "option") return render(item, "".concat(item.index));
				});
				else if (isLoading) {
					var message = loadingMessage({ inputValue });
					if (message === null) return null;
					menuUI = /*#__PURE__*/ import_react.createElement(LoadingMessage, commonProps, message);
				} else {
					var _message = noOptionsMessage({ inputValue });
					if (_message === null) return null;
					menuUI = /*#__PURE__*/ import_react.createElement(NoOptionsMessage, commonProps, _message);
				}
				var menuPlacementProps = {
					minMenuHeight,
					maxMenuHeight,
					menuPlacement,
					menuPosition,
					menuShouldScrollIntoView
				};
				var menuElement = /*#__PURE__*/ import_react.createElement(MenuPlacer, _extends({}, commonProps, menuPlacementProps), function(_ref4) {
					var ref = _ref4.ref, _ref4$placerProps = _ref4.placerProps, placement = _ref4$placerProps.placement, maxHeight = _ref4$placerProps.maxHeight;
					return /*#__PURE__*/ import_react.createElement(Menu, _extends({}, commonProps, menuPlacementProps, {
						innerRef: ref,
						innerProps: {
							onMouseDown: _this4.onMenuMouseDown,
							onMouseMove: _this4.onMenuMouseMove
						},
						isLoading,
						placement
					}), /*#__PURE__*/ import_react.createElement(ScrollManager, {
						captureEnabled: captureMenuScroll,
						onTopArrive: onMenuScrollToTop,
						onBottomArrive: onMenuScrollToBottom,
						lockEnabled: menuShouldBlockScroll
					}, function(scrollTargetRef) {
						return /*#__PURE__*/ import_react.createElement(MenuList, _extends({}, commonProps, {
							innerRef: function innerRef(instance) {
								_this4.getMenuListRef(instance);
								scrollTargetRef(instance);
							},
							innerProps: {
								role: "listbox",
								"aria-multiselectable": commonProps.isMulti,
								id: _this4.getElementId("listbox")
							},
							isLoading,
							maxHeight,
							focusedOption
						}), menuUI);
					}));
				});
				return menuPortalTarget || menuPosition === "fixed" ? /*#__PURE__*/ import_react.createElement(MenuPortal, _extends({}, commonProps, {
					appendTo: menuPortalTarget,
					controlElement: this.controlRef,
					menuPlacement,
					menuPosition
				}), menuElement) : menuElement;
			}
		},
		{
			key: "renderFormField",
			value: function renderFormField() {
				var _this5 = this;
				var _this$props13 = this.props, delimiter = _this$props13.delimiter, isDisabled = _this$props13.isDisabled, isMulti = _this$props13.isMulti, name = _this$props13.name, required = _this$props13.required;
				var selectValue = this.state.selectValue;
				if (required && !this.hasValue() && !isDisabled) return /*#__PURE__*/ import_react.createElement(RequiredInput$1, {
					name,
					onFocus: this.onValueInputFocus
				});
				if (!name || isDisabled) return;
				if (isMulti) if (delimiter) {
					var value = selectValue.map(function(opt) {
						return _this5.getOptionValue(opt);
					}).join(delimiter);
					return /*#__PURE__*/ import_react.createElement("input", {
						name,
						type: "hidden",
						value
					});
				} else {
					var input = selectValue.length > 0 ? selectValue.map(function(opt, i) {
						return /*#__PURE__*/ import_react.createElement("input", {
							key: "i-".concat(i),
							name,
							type: "hidden",
							value: _this5.getOptionValue(opt)
						});
					}) : /*#__PURE__*/ import_react.createElement("input", {
						name,
						type: "hidden",
						value: ""
					});
					return /*#__PURE__*/ import_react.createElement("div", null, input);
				}
				else {
					var _value = selectValue[0] ? this.getOptionValue(selectValue[0]) : "";
					return /*#__PURE__*/ import_react.createElement("input", {
						name,
						type: "hidden",
						value: _value
					});
				}
			}
		},
		{
			key: "renderLiveRegion",
			value: function renderLiveRegion() {
				var commonProps = this.commonProps;
				var _this$state6 = this.state, ariaSelection = _this$state6.ariaSelection, focusedOption = _this$state6.focusedOption, focusedValue = _this$state6.focusedValue, isFocused = _this$state6.isFocused, selectValue = _this$state6.selectValue;
				var focusableOptions = this.getFocusableOptions();
				return /*#__PURE__*/ import_react.createElement(LiveRegion$1, _extends({}, commonProps, {
					id: this.getElementId("live-region"),
					ariaSelection,
					focusedOption,
					focusedValue,
					isFocused,
					selectValue,
					focusableOptions,
					isAppleDevice: this.state.isAppleDevice
				}));
			}
		},
		{
			key: "render",
			value: function render() {
				var _this$getComponents8 = this.getComponents(), Control = _this$getComponents8.Control, IndicatorsContainer = _this$getComponents8.IndicatorsContainer, SelectContainer = _this$getComponents8.SelectContainer, ValueContainer = _this$getComponents8.ValueContainer;
				var _this$props14 = this.props, className = _this$props14.className, id = _this$props14.id, isDisabled = _this$props14.isDisabled, menuIsOpen = _this$props14.menuIsOpen;
				var isFocused = this.state.isFocused;
				var commonProps = this.commonProps = this.getCommonProps();
				return /*#__PURE__*/ import_react.createElement(SelectContainer, _extends({}, commonProps, {
					className,
					innerProps: {
						id,
						onKeyDown: this.onKeyDown
					},
					isDisabled,
					isFocused
				}), this.renderLiveRegion(), /*#__PURE__*/ import_react.createElement(Control, _extends({}, commonProps, {
					innerRef: this.getControlRef,
					innerProps: {
						onMouseDown: this.onControlMouseDown,
						onTouchEnd: this.onControlTouchEnd
					},
					isDisabled,
					isFocused,
					menuIsOpen
				}), /*#__PURE__*/ import_react.createElement(ValueContainer, _extends({}, commonProps, { isDisabled }), this.renderPlaceholderOrValue(), this.renderInput()), /*#__PURE__*/ import_react.createElement(IndicatorsContainer, _extends({}, commonProps, { isDisabled }), this.renderClearIndicator(), this.renderLoadingIndicator(), this.renderIndicatorSeparator(), this.renderDropdownIndicator())), this.renderMenu(), this.renderFormField());
			}
		}
	], [{
		key: "getDerivedStateFromProps",
		value: function getDerivedStateFromProps(props, state) {
			var prevProps = state.prevProps, clearFocusValueOnUpdate = state.clearFocusValueOnUpdate, inputIsHiddenAfterUpdate = state.inputIsHiddenAfterUpdate, ariaSelection = state.ariaSelection, isFocused = state.isFocused, prevWasFocused = state.prevWasFocused, instancePrefix = state.instancePrefix;
			var options = props.options, value = props.value, menuIsOpen = props.menuIsOpen, inputValue = props.inputValue, isMulti = props.isMulti;
			var selectValue = cleanValue(value);
			var newMenuOptionsState = {};
			if (prevProps && (value !== prevProps.value || options !== prevProps.options || menuIsOpen !== prevProps.menuIsOpen || inputValue !== prevProps.inputValue)) {
				var focusableOptions = menuIsOpen ? buildFocusableOptions(props, selectValue) : [];
				var focusableOptionsWithIds = menuIsOpen ? buildFocusableOptionsWithIds(buildCategorizedOptions(props, selectValue), "".concat(instancePrefix, "-option")) : [];
				var focusedValue = clearFocusValueOnUpdate ? getNextFocusedValue(state, selectValue) : null;
				var focusedOption = getNextFocusedOption(state, focusableOptions);
				newMenuOptionsState = {
					selectValue,
					focusedOption,
					focusedOptionId: getFocusedOptionId(focusableOptionsWithIds, focusedOption),
					focusableOptionsWithIds,
					focusedValue,
					clearFocusValueOnUpdate: false
				};
			}
			var newInputIsHiddenState = inputIsHiddenAfterUpdate != null && props !== prevProps ? {
				inputIsHidden: inputIsHiddenAfterUpdate,
				inputIsHiddenAfterUpdate: void 0
			} : {};
			var newAriaSelection = ariaSelection;
			var hasKeptFocus = isFocused && prevWasFocused;
			if (isFocused && !hasKeptFocus) {
				newAriaSelection = {
					value: valueTernary(isMulti, selectValue, selectValue[0] || null),
					options: selectValue,
					action: "initial-input-focus"
				};
				hasKeptFocus = !prevWasFocused;
			}
			if ((ariaSelection === null || ariaSelection === void 0 ? void 0 : ariaSelection.action) === "initial-input-focus") newAriaSelection = null;
			return _objectSpread2(_objectSpread2(_objectSpread2({}, newMenuOptionsState), newInputIsHiddenState), {}, {
				prevProps: props,
				ariaSelection: newAriaSelection,
				prevWasFocused: hasKeptFocus
			});
		}
	}]);
	return Select;
}(import_react.Component);
Select.defaultProps = defaultProps;
//#endregion
//#region node_modules/react-select/dist/react-select.esm.js
var StateManagedSelect$1 = /* @__PURE__ */ (0, import_react.forwardRef)(function(props, ref) {
	var baseSelectProps = useStateManager(props);
	return /*#__PURE__*/ import_react.createElement(Select, _extends({ ref }, baseSelectProps));
});
//#endregion
export { StateManagedSelect$1 as t };
