Shader "Custom/CookTorranceSpec"
{
	Properties
	{
		_MainTex("Texture", 2D) = "white" {}
		_BumpTex("Bump map", 2D) = "bump" {}
		_MetallicTex("Metallic map", 2D) = "white" {}
		_AOTex ("AO map", 2D) = "white" {}
		_Roughness("Roughness", Range(0.0,1.0)) = 1.0
		_SpecularColor("Specular color", Color) = (1,1,1,1)
	}
	SubShader
	{
		Tags { "RenderType"="Opaque" "LightMode" = "ForwardBase"}
		LOD 100

		Pass
		{
			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag
			#pragma target 3.0

			#define PI 3.14159265358979323846
			#define INV_PI 0.31830988618
			
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
			float4 _MainTex_ST;
			float _Roughness;
			float3 _SpecularColor;

			float sqr(float value)
			{
				return value * value;
			}

			float G1(float k, float x)
			{
				return x / (x * (1 - k) + k);
			}
			
			float CookTorranceSpec(float NdotL, float LdotH, float NdotH, float NdotV, float roughness, float specularColor)
			{
				float alpha = sqr(roughness);

				float alphaSqr = sqr(alpha);
				float denominator = sqr(NdotH) * (alphaSqr - 1.0) + 1.0;
				float d = alphaSqr / (PI * sqr(denominator));


				float f = specularColor + (1 - specularColor) * pow(1 - LdotH, 5);
				
				float modifiedRoughness = roughness + 1;
				// k from unreal paper.
				float k = sqr(modifiedRoughness) * 0.125;

				float g1L = G1(k, NdotL);
				float g1V = G1(k, NdotV);
				
				float g = g1L * g1V;

				return NdotL * d * g * f;
			}
			
			v2f vert (appdata v)
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
			
			fixed4 frag (v2f i) : SV_Target
			{
				// sample the texture
				fixed4 col = tex2D(_MainTex, i.uv);
				fixed3 metal = tex2D(_MetallicTex, i.uv);
				fixed3 AO = tex2D(_AOTex, i.uv);

				half3 tnormal = UnpackNormal(tex2D(_BumpTex, i.uv));
				// transform normal from tangent to world space
				half3 normal = mul(i.TtoW, tnormal);

				half3 viewDir = normalize(i.viewDir);
				half3 lightDir = normalize(_WorldSpaceLightPos0.xyz);
				float3 halfV = normalize(lightDir + viewDir);
				float NdotL = max(0, dot(normal, lightDir));
				float NdotH = max(0, dot(normal, halfV));
				float NdotV = max(0, dot(normal, viewDir));
				float VdotH = max(0, dot(viewDir, halfV));
				float LdotH = max(0, dot(lightDir, halfV));

				float3 spec = CookTorranceSpec(NdotL, LdotH, NdotH, NdotV, _Roughness, Luminance(_SpecularColor));

				col.rgb = (col.rgb + spec * _SpecularColor) * _LightColor0.rgb * AO;

				return fixed4(col.rgb, 1);
			}
			ENDCG
		}
	}
}
