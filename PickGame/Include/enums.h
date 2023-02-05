#pragma once

enum SceneObjectType
{
	SOT_Opaque = 'opaq',
	SOT_Transparent = 'tran',
	SOT_SkinnedOpaque = 'skop',
	SOT_SkinnedTransparent = 'sktr',
	SOT_AnimatedEffect = 'anef',
	SOT_Sky = 'sky'
};

enum LayerType
{
	LT_opaque = 0,
	LT_transparent,
	LT_skinnedOpaque,
	LT_skinnedTransparent,
	LT_animatedEfferct,
	LT_sky,
	LT_COUNT
};