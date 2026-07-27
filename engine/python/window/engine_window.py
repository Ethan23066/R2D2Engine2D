import glfw
from engine.cython.window.engine_window import EngineWindow as _EngineWindow

def main():
    # Test Cython
    cy_win = _EngineWindow(800, 600, "Cython Test Window")

    # Test pyglfw
    glfw.init()
    window = glfw.create_window(800, 600, "Python Test Window", None, None)
    glfw.make_context_current(window)

    # Boucle simple
    while not glfw.window_should_close(window):
        glfw.poll_events()
        glfw.swap_buffers(window)

    glfw.destroy_window(window)
    glfw.terminate()

    cy_win.shutdown()

if __name__ == "__main__":
    main()
