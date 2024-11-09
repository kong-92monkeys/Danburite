//? #version 460

#ifndef TEX_UTIL_GLSL
#define TEX_UTIL_GLSL

float TexUtil_wrap(
	const float src)
{
	return mod(src, 1.0f);
}

float TexUtil_clamp(
	const float src)
{
	return clamp(src, 0.0f, 1.0f);
}

float TexUtil_mirror(
	const float src)
{
	const float modSrc = mod(src, 1.0f);

	if ((int(src) % 2) == 0)
		return modSrc;

	return (1.0f - modSrc);
}

bool TexUtil_isValidDecal(
	const float src)
{
	if (src < 0.0f)
		return false;

	if (src >= 1.0f)
		return false;

	return true;
}

#endif