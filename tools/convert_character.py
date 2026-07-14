"""
Run headless: blender --background --python convert_character.py -- <char.fbx> <anim.fbx> <out.glb>

Imports the character FBX and the animation-source FBX (a separate Mixamo download sharing
the same bone names), then bakes the animation source's pose onto the character armature
frame-by-frame (rather than reassigning the Action object across armatures - Blender 4.4+'s
"Action Slots" system doesn't reliably rebind an action's fcurves to a different armature's
pose bones just by setting animation_data.action, which produced a near-zero-length
"animation" frozen at the bind pose). Discards the animation source's own mesh/skeleton, and
exports the character with its mesh + skin + the baked animation as a single glb.
"""
import bpy
import sys

argv = sys.argv[sys.argv.index("--") + 1:]
char_fbx, anim_fbx, out_glb = argv[0], argv[1], argv[2]

bpy.ops.wm.read_factory_settings(use_empty=True)

before = set(bpy.data.objects)
bpy.ops.import_scene.fbx(filepath=char_fbx)
char_objects = set(bpy.data.objects) - before
char_armature = next(o for o in char_objects if o.type == 'ARMATURE')
print(f"[convert] character armature: {char_armature.name}, {len(char_armature.data.bones)} bones")

before2 = set(bpy.data.objects)
bpy.ops.import_scene.fbx(filepath=anim_fbx)
anim_objects = set(bpy.data.objects) - before2
anim_armature = next(o for o in anim_objects if o.type == 'ARMATURE')
print(f"[convert] animation armature: {anim_armature.name}, {len(anim_armature.data.bones)} bones")

if not anim_armature.animation_data or not anim_armature.animation_data.action:
    raise RuntimeError("animation source FBX has no action on its armature")

action = anim_armature.animation_data.action
frame_start, frame_end = int(action.frame_range[0]), int(action.frame_range[1])
print(f"[convert] action: {action.name}, frames {frame_start}-{frame_end}")

shared_bones = [b.name for b in char_armature.pose.bones if b.name in anim_armature.pose.bones]
print(f"[convert] baking {len(shared_bones)} shared bones across {frame_end - frame_start + 1} frames")

char_armature.animation_data_create()
char_action = bpy.data.actions.new(name="ch15_firing_baked")
char_armature.animation_data.action = char_action

scene = bpy.context.scene
for frame in range(frame_start, frame_end + 1):
    scene.frame_set(frame)
    for bone_name in shared_bones:
        src = anim_armature.pose.bones[bone_name]
        dst = char_armature.pose.bones[bone_name]
        dst.matrix_basis = src.matrix_basis.copy()
        dst.keyframe_insert(data_path="location", frame=frame)
        if dst.rotation_mode == 'QUATERNION':
            dst.keyframe_insert(data_path="rotation_quaternion", frame=frame)
        else:
            dst.keyframe_insert(data_path="rotation_euler", frame=frame)
        dst.keyframe_insert(data_path="scale", frame=frame)

scene.frame_start = frame_start
scene.frame_end = frame_end
scene.frame_set(frame_start)
print(f"[convert] baked action frame_range={tuple(char_action.frame_range)}")

bpy.ops.object.select_all(action='DESELECT')
for o in anim_objects:
    o.select_set(True)
bpy.ops.object.delete()

bpy.ops.object.select_all(action='DESELECT')
for o in char_objects:
    o.select_set(True)
bpy.context.view_layer.objects.active = char_armature

bpy.ops.export_scene.gltf(
    filepath=out_glb,
    export_format='GLB',
    use_selection=True,
    export_animations=True,
    export_animation_mode='ACTIONS',
    export_force_sampling=True,
    export_skins=True,
    export_yup=True,
    # The source textures are lossless PNGs (some at 4K), embedded directly into the glb -
    # that's what made the file 123MB. JPEG at quality 80 is a large, visually-cheap win.
    # (Images needing alpha are still saved as PNG automatically - Blender handles that.)
    export_image_format='JPEG',
    export_image_quality=80,
)
print(f"[convert] wrote {out_glb}")
