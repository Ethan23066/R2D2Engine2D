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
        index = 01

    # Récupération de la résolution native du moniteur choisi
    width, height, refresh = EngineWindow.get_monitor_mode(index)
    print(f"Résolution native détectée : {width}x{height} @ {refresh}Hz")

    # Création de la fenêtre fullscreen native
    win = EngineWindow(width, height, "R2D2 Engine", monitor_index=index)
    win.run()
    win.shutdown()

if __name__ == "__main__":
    main()
