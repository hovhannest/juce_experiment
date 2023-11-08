import pyautogui

def test_app_start(start_application, os_resource_getter):
    image_location = pyautogui.locateOnScreen(os_resource_getter("sample/golden/img.png"), confidence=0.9)
    assert(image_location)
