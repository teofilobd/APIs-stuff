Shader "Custom/CookTorranceSpec LUT"
{
	Properties
	{
		_MainTex("Texture", 2D) = "white" {}
		_BumpTex("Bump map", 2D) = "bump" {}
		_MetallicTex("Metallic map", 2D) = "white" {}
		_AOTex("AO map", 2D) = "white" {}
		_Smoothness("Smoothness", Range(0.0,1.0)) = 1.0
		_CookTorranceLUT("Cook Torrance LUT", 2D) = "white" {}
	}
	SubShader
	{
		Tags{ "RenderType" = "Opaque" "LightMode" = "ForwardBase" }
		LOD 100

		Pass
		{
			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag
			#pragma target 3.0

			#include "UnityCG.cginc"
			#include "Lighting.cginc"

			struct appdata
			{
				float4 vertex : POSITION;
				float2 uv : TEXCOORD0;
				float3 normal : NORMAL;
				float4 tangent : TANGENT;
			};

			struct v2f
			{
				float2 uv : TEXCOORD0;
				float4 vertex : SV_POSITION;
				half3x3 TtoW : TEXCOORD2;
				float3 worldPos : TEXCOORD5;
				float3 worldNormal : TEXCOORD6;
				float3 viewDir : TEXCOORD7;
			};

			sampler2D _MainTex;
			sampler2D _BumpTex;
			sampler2D _MetallicTex;
			sampler2D _AOTex;
			sampler2D _CookTorranceLUT;
			float4 _MainTex_ST;
			float _Smoothness;

			v2f vert(appdata v)
			{
				v2f o;
				o.vertex = UnityObjectToClipPos(v.vertex);
				o.uv = TRANSFORM_TEX(v.uv, _MainTex);

				o.viewDir = WorldSpaceViewDir(v.vertex);
				o.worldPos = mul(unity_ObjectToWorld, o.vertex).xyz;
				half3 wNormal = UnityObjectToWorldNormal(v.normal);
				o.worldNormal = wNormal;
				half3 wTangent = UnityObjectToWorldDir(v.tangent.xyz);
				// compute bitangent from cross product of normal and tangent
				half tangentSign = v.tangent.w * unity_WorldTransformParams.w;
				half3 wBitangent = cross(wNormal, wTangent) * tangentSign;
				// output the tangent space matrix
				o.TtoW = half3x3(half3(wTangent.x, wBitangent.x, wNormal.x),
								 half3(wTangent.y, wBitangent.y, wNormal.y),
								 half3(wTangent.z, wBitangent.z, wNormal.z));
				return o;
			}

			fixed4 frag(v2f i) : SV_Target
			{
				fixed4 col = tex2D(_MainTex, i.uv);
				fixed4 metal = tex2D(_MetallicTex, i.uv);
				fixed3 AO = tex2D(_AOTex, i.uv);

				half3 tnormal = UnpackNormal(tex2D(_BumpTex, i.uv));
				half3 normal = mul(i.TtoW, tnormal);

				half3 viewDir = normalize(i.viewDir);
				half3 lightDir = normalize(_WorldSpaceLightPos0.xyz);
				float3 halfV = normalize(lightDir + viewDir);
				float NdotL = max(0, dot(normal, lightDir));
				float NdotH = max(0, dot(normal, halfV));
				float NdotV = max(0, dot(normal, viewDir));
				float LdotH = max(0, dot(lightDir, halfV));

				float roughness = (1 - metal.a * _Smoothness);

				float F = tex2D(_CookTorranceLUT, float2(LdotH, 0.04)).r;
				float G1L = tex2D(_CookTorranceLUT, float2(roughness, NdotL)).g;
				float G1V = tex2D(_CookTorranceLUT, float2(roughness, NdotV)).g;
				float D = tex2D(_CookTorranceLUT, float2(roughness, NdotH)).b;

				float spec = NdotL * F * G1L * G1V * D;

				fixed3 diffuse = 0; // TODO
				
				fixed3 specular =  (1 - metal.rgb) * spec;

				col.rgb = ((diffuse + specular)) * _LightColor0.rgb * AO;

				return fixed4(saturate(col.rgb), 1);
			}
			ENDCG
		}
	}
}
