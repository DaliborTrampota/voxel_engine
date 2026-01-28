float linstep(float low, float high, float v) {
    return clamp((v - low) / (high - low), 0.0, 1.0);
}
