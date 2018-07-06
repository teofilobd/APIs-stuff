using System.IO;
using UnityEditor;
using UnityEngine;

public class CookTorranceBaker : MonoBehaviour
{
    public void Bake()
    {
        int width = 2048;
        int height = 2048;

        Texture2D lut = new Texture2D(width, height, TextureFormat.RGBAFloat, false);
        lut.filterMode = FilterMode.Bilinear;
        lut.wrapMode = TextureWrapMode.Clamp;
        
        for (int x = 0; x < width; ++x)
        {
            float xLerp = (float) x / (width - 1);
            float roughness = xLerp;
            float alpha = roughness * roughness;
            float alphaSqr = alpha * alpha;
            float k = roughness + 1;
            k = k * k * 0.125f;

            float LdotH5 = Mathf.Pow(1 - xLerp, 5);

            // R -> F -> LdotH x Spec
            // G -> G -> Roughness x NdotL
            // G -> G -> Roughness x NdotV
            // B -> D -> Roughness x NdotH

            for (int y = 0; y < height; ++y)
            {
                float yLerp = (float)y / (height - 1);

                float F = yLerp + (1 - yLerp) * LdotH5;

                float denom = yLerp * yLerp * (alphaSqr - 1) + 1;

                denom = Mathf.Max(Mathf.PI * denom * denom, 0.0000001f);
                float D = alphaSqr / denom;
                
                float G = yLerp / Mathf.Max(yLerp * (1 - k) + k, 0.0000001f);

                Color c = new Color(F, G, D, 0);
                lut.SetPixel(x, y, c);
            }
        }
        
        lut.Apply();

        byte[] bytes = lut.EncodeToEXR();

        DestroyImmediate(lut);

        File.WriteAllBytes(Application.dataPath + "/CookTorranceLUT.exr", bytes);
    }
}

[CustomEditor(typeof(CookTorranceBaker))]
public class CookTorranceBakerEditor : Editor
{
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        CookTorranceBaker ctb = target as CookTorranceBaker;

        if(GUILayout.Button("Bake"))
        {
            ctb.Bake();
        }
    }
}


