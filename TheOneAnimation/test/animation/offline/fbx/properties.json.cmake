{
  "skeleton":
  {
    "filename":"${ozz_temp_directory}/fbx_propreties_skeleton.ozz"
  },
  "animations":
  [
    {
      "filename":"${ozz_temp_directory}/fbx_properties_animation_${CMAKE_CURRENT_LIST_LINE}.ozz",
      "tracks":
      [
        {
          "properties":
          [
            {
              "filename":"${ozz_temp_directory}/fbx_properties_track1_*.ozz",
              "joint_name":"*",
              "property_name":"ozz_*",
              "type":"float1"
            },
            {
              "filename":"${ozz_temp_directory}/fbx_properties_track2_*.ozz",
              "joint_name":"*",
              "property_name":"ozz_*",
              "type":"float2"
            },
            {
              "filename":"${ozz_temp_directory}/fbx_properties_track3_*.ozz",
              "joint_name":"*",
              "property_name":"ozz_*",
              "type":"float3"
            },
            {
              "filename":"${ozz_temp_directory}/fbx_properties_track3p_*.ozz",
              "joint_name":"*",
              "property_name":"ozz_*",
              "type":"point"
            },
            {
              "filename":"${ozz_temp_directory}/fbx_properties_track3v_*.ozz",
              "joint_name":"*",
              "property_name":"ozz_*",
              "type":"vector"
            },
            {
              "filename":"${ozz_temp_directory}/fbx_properties_track4_*.ozz",
              "joint_name":"*",
              "property_name":"ozz_*",
              "type":"float4"
            }
          ]
        }
      ]
    }
  ]
}