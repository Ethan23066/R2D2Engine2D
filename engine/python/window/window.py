from engine.cython.window.window import EngineWindow

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

    width, height, refresh = EngineWindow.get_monitor_mode(index)
    print(f"Résolution native détectée : {width}x{height} @ {refresh}Hz")


    print("Modes disponibles :")
    print("0 → Fenêtré")
    print("1 → Fullscreen exclusif")


    try:
        mode = int(input("Choisis un mode : "))
    except ValueError:
        mode = 0

    win = EngineWindow(width, height, "R2D2 Engine", monitor_index=index, mode=mode)
    win.run()
    win.shutdown()

if __name__ == "__main__":
    main()
