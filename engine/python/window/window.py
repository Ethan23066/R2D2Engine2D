from engine.cython.window.window import EngineWindow
import argparse
import logging
import sys

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


def main(argv=None):
    parser = argparse.ArgumentParser(prog="r2d2-window", description="Lancer la fenêtre R2D2 Engine")
    parser.add_argument("--monitor", "-m", type=int, help="Index du moniteur (par défaut : sélection interactive)")
    args = parser.parse_args(argv)

    monitors = EngineWindow.list_monitors()
    if not monitors:
        logger.error("Aucun écran détecté.")
        return 1

    logger.info("Écrans disponibles :")
    for i, name in enumerate(monitors):
        logger.info("%d → %s", i, name)

    index = args.monitor
    if index is None:
        try:
            index = int(input("Choisis un écran (index) : "))
        except (ValueError, EOFError):
            index = 0

    if index < 0 or index >= len(monitors):
        logger.warning("Index invalide, utilisation de 0.")
        index = 0

    try:
        width, height, refresh = EngineWindow.get_monitor_mode(index)
    except Exception as exc:
        logger.exception("Impossible de récupérer la résolution du moniteur : %s", exc)
        return 1

    logger.info("Résolution native détectée : %dx%d @ %dHz", width, height, refresh)

    win = EngineWindow(width, height, "R2D2 Engine", monitor_index=index)
    try:
        win.run()
    finally:
        try:
            win.shutdown()
        except Exception:
            logger.exception("Erreur lors de la fermeture de la fenêtre")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
