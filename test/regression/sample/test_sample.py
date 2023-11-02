import pyautogui

def test_app_start(start_application):
    image_location = pyautogui.locateOnScreen("sample/golden/img.png", confidence=0.9)
    assert(image_location)
