// Returns a color interpolated between minColor and maxColor based on value between minValue and maxValue
function colorForRange(value, minValue, maxValue, minColor, maxColor) {
    if (value < minValue)
        value = minValue
    if (value > maxValue)
        value = maxValue

    function toRgb(color) {
        if (typeof color === "object" && "r" in color) {
            return {
                "r": color.r,
                "g": color.g,
                "b": color.b
            }
        }

        if (typeof color === "string") {
            let hex = color.replace("#", "")
            return {
                "r": parseInt(hex.substr(0, 2), 16) / 255,
                "g": parseInt(hex.substr(2, 2), 16) / 255,
                "b": parseInt(hex.substr(4, 2), 16) / 255
            }
        }
        console.warn("Unknown color format", color)
        return {
            "r": 0,
            "g": 0,
            "b": 0
        }
    }

    let ratio = (value - minValue) / (maxValue - minValue)
    let cMin = toRgb(minColor)
    let cMax = toRgb(maxColor)

    let r = cMin.r + ratio * (cMax.r - cMin.r)
    let g = cMin.g + ratio * (cMax.g - cMin.g)
    let b = cMin.b + ratio * (cMax.b - cMin.b)

    return Qt.rgba(r, g, b, 1)
}
