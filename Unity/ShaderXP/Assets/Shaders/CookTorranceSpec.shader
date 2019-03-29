Shader "Custom/CookTorranceSpec"
{
	Properties
	{
		_MainTex("Texture", 2D) = "white" {}
		_BumpTex("Bump map", 2D) = "bump" {}
		_MetallicTex("Metallic map", 2D) = "white" {}
		_AOTex ("AO map", 2D) = "white" {}
		_Smoothness("Smoothness", Range(0.0,1.0)) = 1.0
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
				float3x3 TtoW : TEXCOORD2;
				float3 worldPos : TEXCOORD5;
				float3 worldNormal : TEXCOORD6;
				float3 viewDir : TEXCOORD7;
			};

			sampler2D _MainTex;
			sampler2D _BumpTex;
			sampler2D _MetallicTex;
			sampler2D _AOTex;
			float4 _MainTex_ST;
			float _Smoothness;
			float3 _SpecularColor;

			float sqr(float value)
			{
				return value * value;
			}

			float G1(float k, float x)
			{
				return x / (x * (1 - k) + k);
			}
			
			float CookTorranceSpec(float NdotL, float LdotH, float NdotH, float NdotV, float roughness, float f0)
			{
				float alpha = sqr(roughness);

				float alphaSqr = sqr(alpha);
				float denominator = sqr(NdotH) * (alphaSqr - 1.0) + 1.0;
				float d = alphaSqr / (PI * sqr(denominator));

				float f = f0 + (1 - f0) * pow(1 - LdotH, 5);
				
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
				float3 wNormal = UnityObjectToWorldNormal(v.normal);
				o.worldNormal = wNormal;
				float3 wTangent = UnityObjectToWorldDir(v.tangent.xyz);
				// compute bitangent from cross product of normal and tangent
				float tangentSign = v.tangent.w * unity_WorldTransformParams.w;
				float3 wBitangent = cross(wNormal, wTangent) * tangentSign;
				// output the tangent space matrix
				o.TtoW = float3x3(float3(wTangent.x, wBitangent.x, wNormal.x),
								 float3(wTangent.y, wBitangent.y, wNormal.y),
								 float3(wTangent.z, wBitangent.z, wNormal.z));
				return o;
			}
			
			float4 frag (v2f i) : SV_Target
			{
				// sample the texture
				float4 col = tex2D(_MainTex, i.uv);
				float4 metal = tex2D(_MetallicTex, i.uv);
				float3 AO = tex2D(_AOTex, i.uv);

				float3 tnormal = UnpackNormal(tex2D(_BumpTex, i.uv));
				// transform normal from tangent to world space
				float3 normal = mul(i.TtoW, tnormal);

				float3 viewDir = normalize(i.viewDir);
				float3 lightDir = normalize(_WorldSpaceLightPos0.xyz);
				float3 floatV = normalize(lightDir + viewDir);
				float NdotL = max(0, dot(normal, lightDir));
				float NdotH = max(0, dot(normal, floatV));
				float NdotV = max(0, dot(normal, viewDir));
				float VdotH = max(0, dot(viewDir, floatV));
				float LdotH = max(0, dot(lightDir, floatV));

				float roughness = (1 - metal.a * _Smoothness);

				float spec = CookTorranceSpec(NdotL, LdotH, NdotH, NdotV, roughness, 0.04);

				float3 diffuse = 0; // TODO

				float3 specular = (1 - metal.rgb) * spec;

				col.rgb = ((diffuse + specular)) * _LightColor0.rgb * AO;

				return float4(col.rgb, 1);
			}
			ENDCG
		}
	}
}
