using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExecuteInEditMode]
public class MaterialController : MonoBehaviour
{
    public Material UnityMaterial;
    public Material CTMaterial;
    public Material LUTMaterial;

    [Range(0.0f,1.0f)]
    public float Smoothness;

	// Update is called once per frame
	void Update ()
    {
	    if(UnityMaterial != null)
        {
            UnityMaterial.SetFloat("_GlossMapScale", Smoothness);
        }
        if(CTMaterial != null)
        {
            CTMaterial.SetFloat("_Smoothness", Smoothness);
        }
        if(LUTMaterial != null)
        {
            LUTMaterial.SetFloat("_Smoothness", Smoothness);
        }
	}
}
