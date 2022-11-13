R""(
float GetRoughnessFade()
{
	// mask SSR to reduce noise and for better performance, roughness of 0 should have SSR, at MaxRoughness we fade to 0
	return min(sqrt(Alpha()) * ROUGHNESSMASKSCALE + 2, 1.0);
}

void	SSRStencil() {
	if (Depth() == 1 || GetRoughnessFade() < 0.01)
		discard;
	else
		gl_FragDepth = 0;
}
)""