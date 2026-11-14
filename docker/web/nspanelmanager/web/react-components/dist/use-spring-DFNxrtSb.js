import { c as __toESM, r as require_react } from "./main-2GKhVwa3.js";
import { a as isMotionValue, c as JSAnimation, d as frame, l as interpolate, m as useConstant, o as collectMotionValues, p as useIsomorphicLayoutEffect, r as MotionConfigContext, s as motionValue, u as cancelFrame } from "./proxy-BJsAbzcZ.js";
//#region node_modules/motion-dom/dist/es/utils/transform.mjs
function transform(...args) {
	const useImmediate = !Array.isArray(args[0]);
	const argOffset = useImmediate ? 0 : -1;
	const inputValue = args[0 + argOffset];
	const inputRange = args[1 + argOffset];
	const outputRange = args[2 + argOffset];
	const options = args[3 + argOffset];
	const interpolator = interpolate(inputRange, outputRange, options);
	return useImmediate ? interpolator(inputValue) : interpolator;
}
//#endregion
//#region node_modules/motion-dom/dist/es/value/follow-value.mjs
/**
* Attach an animation to a MotionValue that will animate whenever the value changes.
* Similar to attachSpring but supports any transition type (spring, tween, inertia, etc.)
*
* @param value - The MotionValue to animate
* @param source - Initial value or MotionValue to track
* @param options - Animation transition options
* @returns Cleanup function
*
* @public
*/
function attachFollow(value, source, options = {}) {
	const initialValue = value.get();
	let activeAnimation = null;
	let latestValue = initialValue;
	let latestSetter;
	const unit = typeof initialValue === "string" ? initialValue.replace(/[\d.-]/g, "") : void 0;
	const stopAnimation = () => {
		if (activeAnimation) {
			activeAnimation.stop();
			activeAnimation = null;
		}
		value.animation = void 0;
	};
	const startAnimation = () => {
		const currentValue = asNumber(value.get());
		const targetValue = asNumber(latestValue);
		if (currentValue === targetValue) {
			stopAnimation();
			return;
		}
		const velocity = activeAnimation ? activeAnimation.getGeneratorVelocity() : value.getVelocity();
		stopAnimation();
		activeAnimation = new JSAnimation({
			keyframes: [currentValue, targetValue],
			velocity,
			type: "spring",
			restDelta: .001,
			restSpeed: .01,
			...options,
			onUpdate: latestSetter
		});
	};
	const scheduleAnimation = () => {
		startAnimation();
		value.animation = activeAnimation ?? void 0;
		value["events"].animationStart?.notify();
		activeAnimation?.then(() => {
			value.animation = void 0;
			value["events"].animationComplete?.notify();
		});
	};
	value.attach((v, set) => {
		latestValue = v;
		latestSetter = (latest) => set(parseValue(latest, unit));
		frame.postRender(scheduleAnimation);
	}, stopAnimation);
	if (isMotionValue(source)) {
		let skipNextAnimation = options.skipInitialAnimation === true;
		const removeSourceOnChange = source.on("change", (v) => {
			if (skipNextAnimation) {
				skipNextAnimation = false;
				value.jump(parseValue(v, unit), false);
			} else value.set(parseValue(v, unit));
		});
		const removeValueOnDestroy = value.on("destroy", removeSourceOnChange);
		return () => {
			removeSourceOnChange();
			removeValueOnDestroy();
		};
	}
	return stopAnimation;
}
function parseValue(v, unit) {
	return unit ? v + unit : v;
}
function asNumber(v) {
	return typeof v === "number" ? v : parseFloat(v);
}
//#endregion
//#region node_modules/framer-motion/dist/es/value/use-motion-value.mjs
var import_react = /* @__PURE__ */ __toESM(require_react(), 1);
/**
* Creates a `MotionValue` to track the state and velocity of a value.
*
* Usually, these are created automatically. For advanced use-cases, like use with `useTransform`, you can create `MotionValue`s externally and pass them into the animated component via the `style` prop.
*
* ```jsx
* export const MyComponent = () => {
*   const scale = useMotionValue(1)
*
*   return <motion.div style={{ scale }} />
* }
* ```
*
* @param initial - The initial state.
*
* @public
*/
function useMotionValue(initial) {
	const value = useConstant(() => motionValue(initial));
	/**
	* If this motion value is being used in static mode, like on
	* the Framer canvas, force components to rerender when the motion
	* value is updated.
	*/
	const { isStatic } = (0, import_react.useContext)(MotionConfigContext);
	if (isStatic) {
		const [, setLatest] = (0, import_react.useState)(initial);
		(0, import_react.useEffect)(() => value.on("change", setLatest), []);
	}
	return value;
}
//#endregion
//#region node_modules/framer-motion/dist/es/value/use-combine-values.mjs
function useCombineMotionValues(values, combineValues) {
	/**
	* Initialise the returned motion value. This remains the same between renders.
	*/
	const value = useMotionValue(combineValues());
	/**
	* Create a function that will update the template motion value with the latest values.
	* This is pre-bound so whenever a motion value updates it can schedule its
	* execution in Framesync. If it's already been scheduled it won't be fired twice
	* in a single frame.
	*/
	const updateValue = () => value.set(combineValues());
	/**
	* Synchronously update the motion value with the latest values during the render.
	* This ensures that within a React render, the styles applied to the DOM are up-to-date.
	*/
	updateValue();
	/**
	* Subscribe to all motion values found within the template. Whenever any of them change,
	* schedule an update.
	*/
	useIsomorphicLayoutEffect(() => {
		const scheduleUpdate = () => frame.preRender(updateValue, false, true);
		const subscriptions = values.map((v) => v.on("change", scheduleUpdate));
		return () => {
			subscriptions.forEach((unsubscribe) => unsubscribe());
			cancelFrame(updateValue);
		};
	});
	return value;
}
//#endregion
//#region node_modules/framer-motion/dist/es/value/use-motion-template.mjs
/**
* Combine multiple motion values into a new one using a string template literal.
*
* ```jsx
* import {
*   motion,
*   useSpring,
*   useMotionValue,
*   useMotionTemplate
* } from "framer-motion"
*
* function Component() {
*   const shadowX = useSpring(0)
*   const shadowY = useMotionValue(0)
*   const shadow = useMotionTemplate`drop-shadow(${shadowX}px ${shadowY}px 20px rgba(0,0,0,0.3))`
*
*   return <motion.div style={{ filter: shadow }} />
* }
* ```
*
* @public
*/
function useMotionTemplate(fragments, ...values) {
	/**
	* Create a function that will build a string from the latest motion values.
	*/
	const numFragments = fragments.length;
	function buildValue() {
		let output = ``;
		for (let i = 0; i < numFragments; i++) {
			output += fragments[i];
			const value = values[i];
			if (value) output += isMotionValue(value) ? value.get() : value;
		}
		return output;
	}
	return useCombineMotionValues(values.filter(isMotionValue), buildValue);
}
//#endregion
//#region node_modules/framer-motion/dist/es/value/use-computed.mjs
function useComputed(compute) {
	/**
	* Open session of collectMotionValues. Any MotionValue that calls get()
	* will be saved into this array.
	*/
	collectMotionValues.current = [];
	compute();
	const value = useCombineMotionValues(collectMotionValues.current, compute);
	/**
	* Synchronously close session of collectMotionValues.
	*/
	collectMotionValues.current = void 0;
	return value;
}
//#endregion
//#region node_modules/framer-motion/dist/es/value/use-transform.mjs
function useTransform(input, inputRangeOrTransformer, outputRangeOrMap, options) {
	if (typeof input === "function") return useComputed(input);
	if (outputRangeOrMap !== void 0 && !Array.isArray(outputRangeOrMap) && typeof inputRangeOrTransformer !== "function") return useMapTransform(input, inputRangeOrTransformer, outputRangeOrMap, options);
	const transformer = typeof inputRangeOrTransformer === "function" ? inputRangeOrTransformer : transform(inputRangeOrTransformer, outputRangeOrMap, options);
	const result = Array.isArray(input) ? useListTransform(input, transformer) : useListTransform([input], ([latest]) => transformer(latest));
	const inputAccelerate = !Array.isArray(input) ? input.accelerate : void 0;
	if (inputAccelerate && !inputAccelerate.isTransformed && typeof inputRangeOrTransformer !== "function" && Array.isArray(outputRangeOrMap) && options?.clamp !== false) result.accelerate = {
		...inputAccelerate,
		times: inputRangeOrTransformer,
		keyframes: outputRangeOrMap,
		isTransformed: true,
		...options?.ease ? { ease: options.ease } : {}
	};
	return result;
}
function useListTransform(values, transformer) {
	const latest = useConstant(() => []);
	return useCombineMotionValues(values, () => {
		latest.length = 0;
		const numValues = values.length;
		for (let i = 0; i < numValues; i++) latest[i] = values[i].get();
		return transformer(latest);
	});
}
function useMapTransform(inputValue, inputRange, outputMap, options) {
	/**
	* Capture keys once to ensure hooks are called in consistent order.
	*/
	const keys = useConstant(() => Object.keys(outputMap));
	const output = useConstant(() => ({}));
	for (const key of keys) output[key] = useTransform(inputValue, inputRange, outputMap[key], options);
	return output;
}
//#endregion
//#region node_modules/framer-motion/dist/es/value/use-follow-value.mjs
function useFollowValue(source, options = {}) {
	const { isStatic } = (0, import_react.useContext)(MotionConfigContext);
	const getFromSource = () => isMotionValue(source) ? source.get() : source;
	if (isStatic) return useTransform(getFromSource);
	const value = useMotionValue(getFromSource());
	(0, import_react.useInsertionEffect)(() => {
		return attachFollow(value, source, options);
	}, [value, JSON.stringify(options)]);
	return value;
}
//#endregion
//#region node_modules/framer-motion/dist/es/value/use-spring.mjs
function useSpring(source, options = {}) {
	return useFollowValue(source, {
		type: "spring",
		...options
	});
}
//#endregion
export { useMotionTemplate as n, useMotionValue as r, useSpring as t };
