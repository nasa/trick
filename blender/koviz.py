import bpy
import socket
import threading
 
# To test
# Add koviz.py to blender addons e.g.:
#    % cd /home/kvetter/.config/blender/2.91/scripts/addons/modules
#    % ln -s /home/kvetter/dev/koviz/python/koviz.py koviz.py
# Bring up blender in a terminal and call helloworld script
#    % blender -P /home/kvetter/dev/koviz/blender/koviz-hello-world.py
# Blender > Animation
# Bring up koviz on ball sim data (see 300.0 below for other data)
#    Click a curve and move mouse to see time update
#    Should see blender animation frame updating
# Press Blender Animation play button
#    Should see koviz time updating
# In Blender, hit <ESC> key to close koviz server

def go():
    # Register the operator.
    bpy.utils.register_class(KovizOperator)
    bpy.ops.object.koviz_operator()
      
def koviz_animation_handler(scene):
    t = get_blender_time()
    KovizOperator.thread.set_koviz_time(t)

def get_blender_time():
    t = None
    frame_current = bpy.context.scene.frame_current
    frame_start = bpy.context.scene.frame_start
    frame_end = bpy.context.scene.frame_end
    nframes = frame_end - frame_start + 1
    data_start = KovizOperator.get_data_start_time()
    data_end = KovizOperator.get_data_end_time()   
    dy = data_end - data_start
    dx = frame_end - frame_start
    if dx > 0 :
        m = dy/dx
        t = m*(frame_current - frame_start) + data_start   
    return t

class KovizThread(threading.Thread):
  
  port = 64053
  max_msg_size = 1024
     
  def __init__(self):
    threading.Thread.__init__(self)
    self.koviz_time = None
    self.is_running = False
    self.client_sock = None
    self.data = [0] * self.max_msg_size
     
  def start(self):
    self.is_running = True
    self.koviz_time = None
    threading.Thread.start(self)
 
  def stop(self):
    self.koviz_time = None
    self.is_running = False

  def get_koviz_time(self):
      return self.koviz_time
  
  def set_koviz_time(self,t):
      if not isinstance(t,float) or self.client_sock == None:
          return
      
      if t != self.koviz_time:
          self.koviz_time = t
          msg = 'TIME={}'.format(t)
          self.client_sock.send(msg.encode())
          
  @staticmethod
  def isfloat(val):
      try:
          float(val)
          return True
      except ValueError:
          return False
            
  def run(self):
    listen_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listen_sock.settimeout(2)
    listen_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    listen_sock.bind(('127.0.0.1', self.port))
    listen_sock.listen(1)
    
    while self.is_running:
        
        self.client_sock, addr = None, None
        try:
            print('Listen for koviz connection')
            self.client_sock,addr = listen_sock.accept()
            self.client_sock.settimeout(2)
            print('Accept koviz connection!')
        except socket.timeout:
            continue
        except OSError:
            self.is_running = False
        
        # Receive koviz client msgs
        while self.is_running:
            try:
                self.data = self.client_sock.recv(self.max_msg_size).decode()
            except socket.timeout:
                continue
            
            words = self.data.split('=')
            if len(words) == 2:
                if words[0] == 't' and self.isfloat(words[1]):
                    self.koviz_time = words[1]
                    # print('KovizThread.koviz_time={}'.format(self.koviz_time))
            elif len(self.data) == 0:
                # Client disconnected    
                break
            
        print('Close koviz connection')
        self.client_sock.close()
        self.koviz_time = None
        
    listen_sock.close()
    print('Closed koviz server!')
    koviz_time = None
    
 
class KovizOperator(bpy.types.Operator) :
 
  bl_idname = 'object.koviz_operator'
  bl_label = 'Koviz Operator'
 
  thread = KovizThread()
  timer = None
  
  # TIM change this (comes from *.trk)
  @staticmethod
  def get_data_start_time():
      return 0.0
  
  # TIM change this (comes from *.trk)
  @staticmethod
  def get_data_end_time():
      return 300.0
  
  
  @staticmethod
  def get_anim_frame(t):
      if t == None :
          return bpy.context.scene.frame_current
      frame = None
      frame_start = bpy.context.scene.frame_start
      frame_end = bpy.context.scene.frame_end
      data_start = KovizOperator.get_data_start_time()
      data_end = KovizOperator.get_data_end_time()   
      dy = frame_end - frame_start
      dx = data_end - data_start
      if dx > 0 :
          m = dy/dx
          frame = m*(float(t) - data_start) + frame_start
      return round(frame)

   
  def modal(self, context, event):
    
    if event.type == 'ESC':
      context.window_manager.event_timer_remove(self.timer)
      KovizOperator.thread.stop()
      if koviz_animation_handler in bpy.app.handlers.frame_change_pre:
          bpy.app.handlers.frame_change_pre.remove(koviz_animation_handler)
      return {'CANCELLED'}
 
    if event.type == 'TIMER':
        koviz_time = KovizOperator.thread.get_koviz_time()
        blender_time = get_blender_time()
        
        koviz_frame = self.get_anim_frame(koviz_time)
        blender_frame = bpy.context.scene.frame_current

        if koviz_time != None and koviz_frame != blender_frame :
            frame = self.get_anim_frame(koviz_time)
            bpy.context.scene.frame_set(frame)
        
        #t = KovizOperator.get_blender_time()
        #print('sphere.pos=(%g)' % bpy.data.objects['Sphere'].location.x)
        #bpy.data.objects['cube'].location = self.thread.data[:2]
        #bpy.data.objects['cube'].rotation_quaternion = self.thread.data[3:]
      
    return {'PASS_THROUGH'}
 
  def execute(self, context):
    
    KovizOperator.thread.start()
    
    self.timer = context.window_manager.event_timer_add(0.2,window=context.window)
    context.window_manager.modal_handler_add(self)

    bpy.app.handlers.frame_change_pre.append(koviz_animation_handler)    
    return {'RUNNING_MODAL'}
