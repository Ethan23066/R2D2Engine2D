from engine.cython.window.engine_window import EngineWindow

def main():
    monitors = EngineWindow.list_monitors()
    if not monitors:
        print("Aucun écran détecté.")
        return

    print("Écrans disponibles :")
    for i, name in enumerate(monitors):
        print(f"{i} → {name}")

    try:
        index = int(input("Choisis un écran (index) : "))
    except ValueError:
        index = 0

    if index < 0 or index >= len(monitors):
        index = 0

    win = EngineWindow(1280, 720, "R2D2 Engine", monitor_index=index)
    win.run()
    win.shutdown()

if __name__ == "__main__":
    main()
