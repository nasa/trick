#!/usr/bin/env python3.11
import vtk

# --- Helper function to make a cone actor with its tip at the origin ---
def make_cone(height=5.0, radius=1.0, color=(1, 0, 0)):
    """
    Create a vtkActor shaped like a cone with its tip at the origin,
    oriented along +Y (so "up" is away from the joint).
    """
    cone = vtk.vtkConeSource()
    cone.SetHeight(height)
    cone.SetRadius(radius)
    cone.SetResolution(32)
    cone.SetDirection(0, 1, 0)  # point along +Y
    cone.SetCenter(0, height/2.0, 0)  # tip at (0,0,0)

    mapper = vtk.vtkPolyDataMapper()
    mapper.SetInputConnection(cone.GetOutputPort())

    actor = vtk.vtkActor()
    actor.SetMapper(mapper)
    actor.GetProperty().SetColor(color)

    return actor


# --- Build assemblies ---
# Shoulder assembly (root)
shoulder_assembly = vtk.vtkAssembly()

# Upper arm (cone)
upper_arm = make_cone(height=5.0, radius=0.7, color=(0.8, 0.3, 0.3))
shoulder_assembly.AddPart(upper_arm)

# Elbow assembly (positioned at bottom of upper arm)
elbow_assembly = vtk.vtkAssembly()
elbow_assembly.SetPosition(0, 5.0, 0)  # move to tip of upper arm (cone height)
shoulder_assembly.AddPart(elbow_assembly)

# Lower arm (cone)
lower_arm = make_cone(height=5.0, radius=0.5, color=(0.3, 0.3, 0.8))
elbow_assembly.AddPart(lower_arm)

# Wrist assembly (positioned at bottom of lower arm)
wrist_assembly = vtk.vtkAssembly()
wrist_assembly.SetPosition(0, 5.0, 0)  # move to tip of lower arm
elbow_assembly.AddPart(wrist_assembly)

# Hand (cone)
hand = make_cone(height=3.0, radius=0.6, color=(0.3, 0.8, 0.3))
wrist_assembly.AddPart(hand)


# --- Add multiple fingers ---
finger_assemblies = []  # keep references for animation

num_fingers = 5
spacing = 0.5  # horizontal spacing between fingers

for i in range(num_fingers):
    # Base knuckle offset along X
    x_offset = (i - (num_fingers - 1) / 2.0) * spacing

    # First knuckle assembly
    finger_base = vtk.vtkAssembly()
    finger_base.SetPosition(x_offset, 3.0, 0)  # attach at top of hand
    wrist_assembly.AddPart(finger_base)

    # First finger segment
    finger_segment1 = make_cone(height=2.0, radius=0.2, color=(0.9, 0.9, 0.2))
    finger_base.AddPart(finger_segment1)

    # Second joint assembly
    finger_joint = vtk.vtkAssembly()
    finger_joint.SetPosition(0, 2.0, 0)  # move to tip of first segment
    finger_base.AddPart(finger_joint)

    # Second finger segment
    finger_segment2 = make_cone(height=1.5, radius=0.15, color=(0.95, 0.95, 0.5))
    finger_joint.AddPart(finger_segment2)

    # Keep both assemblies so we can animate later
    finger_assemblies.append((finger_base, finger_joint))


# --- Renderer / Window ---
renderer = vtk.vtkRenderer()
renderer.SetBackground(0.1, 0.1, 0.1)

render_window = vtk.vtkRenderWindow()
render_window.AddRenderer(renderer)
render_window.SetSize(800, 600)

interactor = vtk.vtkRenderWindowInteractor()
style = vtk.vtkInteractorStyleTrackballCamera()
interactor.SetInteractorStyle(style)
interactor.SetRenderWindow(render_window)

renderer.AddActor(shoulder_assembly)


# --- Simple animation: rotate joints ---
frame = {"count": 0}

def timer_callback(obj, event):
    frame["count"] += 1

    # Rotate shoulder and elbow slowly
    shoulder_assembly.RotateZ(1)
    elbow_assembly.RotateZ(-2)
    wrist_assembly.RotateZ(1.5)

    # Animate fingers (bend back and forth)
    angle = 15 if (frame["count"] // 50) % 2 == 0 else -15
    for base, joint in finger_assemblies:
        base.RotateX(angle)
        joint.RotateX(angle * 0.8)

    render_window.Render()


interactor.AddObserver("TimerEvent", timer_callback)
interactor.CreateRepeatingTimer(50)  # 100 ms per frame

# --- Start ---
render_window.Render()
interactor.Start()
