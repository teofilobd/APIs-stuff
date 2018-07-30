Shader "Unlit/FlowMap"
{
	Properties
	{
		[NoScaleOffset]_MainTex("Texture", 2D) = "white" {}
		_FlowSpeed("Flow speed", float) = 1
		_LerpSpeed("Lerp speed", float) = 1

		[Toggle(DIRECTIONAL_FLOW)]_DirectionalFlow("Directional Flow", Float) = 0
		_FlowDirection("Flow Direction", Vector) = (0,0,0,0)

		[Toggle(RADIAL_FLOW)]_RadialFlow("Radial Flow", Float) = 0	
		_CenterPosition("Center Position", Vector) = (0.5,0.5,0,0)
		
		[Toggle(TEXTURE_FLOW)]_TextureFlow("Texture Flow", Float) = 0
		[NoScaleOffset]_FlowMapTex("Flow map", 2D) = "white" {}

		[NoScaleOffset]_AdditiveTex("Additive Texture", 2D) = "white" {}
		[NoScaleOffset]_BorderMaskTex("Border Opacity", 2D) = "white" {}

		_AngularSpeed("Angular Speed", Float) = 0
		[HDR]_Color1("Color 1", Color) = (1,1,1,1)
		[HDR]_Color2("Color 2", Color) = (1,1,1,1)		
	}
	SubShader
	{
		Tags { "RenderType"="Transparent" "Queue"="Transparent"}
		
		Blend SrcAlpha One
		ZTest LEqual
		ZWrite Off

		Pass
		{
			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag
			#pragma shader_feature __ DIRECTIONAL_FLOW RADIAL_FLOW TEXTURE_FLOW
			
			#include "UnityCG.cginc"

			struct appdata
			{
				float4 vertex : POSITION;
				float2 uv : TEXCOORD0;
			};

			struct v2f
			{
				float2 uv : TEXCOORD0;
				float4 vertex : SV_POSITION;
			};

			sampler2D _MainTex;			
			sampler2D _BorderMaskTex;
			sampler2D _AdditiveTex;
			sampler2D _FlowMapTex;
			uniform float _FlowSpeed;
			uniform float _LerpSpeed;
			uniform float2 _FlowDirection;
			uniform float2 _CenterPosition;
			uniform float4 _Color1;
			uniform float4 _Color2;
			uniform float _AngularSpeed;

			v2f vert (appdata v)
			{
				v2f o;
				o.vertex = UnityObjectToClipPos(v.vertex);
				o.uv = v.uv;
				return o;
			}
			
			fixed4 frag (v2f i) : SV_Target
			{
				fixed4 col = tex2D(_MainTex, i.uv);

				float2 flowDir = float2(0, 0);

#if DIRECTIONAL_FLOW
				flowDir = normalize(_FlowDirection);
#elif RADIAL_FLOW
				flowDir = normalize(i.uv - _CenterPosition);
#elif TEXTURE_FLOW
				flowDir = tex2D(_FlowMapTex, i.uv)*0.5 + 1;
#endif

				flowDir *= _FlowSpeed;

				float timeLerp = _Time.y * _LerpSpeed;
				float phase0 = frac(timeLerp);
				float phase1 = frac(timeLerp + 0.5);

				float2 uv1 = i.uv + phase0 * flowDir;
				float2 uv2 = i.uv + phase1 * flowDir;
				
#if RADIAL_FLOW
				float dist1 = distance(uv1, _CenterPosition);
				float dist2 = distance(uv2, _CenterPosition);
				float timeAng = _Time.y * _AngularSpeed;

				float angSpeed = timeAng * dist1;
				float cosAng = cos(angSpeed);
				float sinAng = sin(angSpeed);					
				float2x2 rotMat1 = float2x2(cosAng, -sinAng, sinAng, cosAng);
				
				angSpeed = timeAng * dist2;
				cosAng = cos(angSpeed);
				sinAng = sin(angSpeed);
				float2x2 rotMat2 = float2x2(cosAng, -sinAng, sinAng, cosAng);

				uv1 = _CenterPosition + mul(rotMat1, uv1 - _CenterPosition);
				uv2 = _CenterPosition + mul(rotMat2, uv2 - _CenterPosition);
#endif

				float lerpV = abs((0.5f - phase0) * 2);

				fixed4 tex1 = tex2D(_MainTex, uv1);
				fixed4 tex2 = tex2D(_MainTex, uv2);
				fixed4 finalCol = lerp(tex1, tex2, lerpV);

				float2 offsetUV = i.uv - _CenterPosition + float2(0.5, 0.5);
				fixed4 additiveTex = tex2D(_AdditiveTex, offsetUV);
				fixed4 borderMask = tex2D(_BorderMaskTex, offsetUV);

				return fixed4(lerp(_Color1, _Color2, distance(i.uv, _CenterPosition) * 2) * (additiveTex.rgb + finalCol.rgb), borderMask.r * (borderMask.r + finalCol.r));
			}
			ENDCG
		}
	}
}