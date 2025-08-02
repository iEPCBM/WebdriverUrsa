import sys
import time
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.chrome.options import Options
from selenium.common.exceptions import NoSuchElementException

def save_screenshot(driver: webdriver.Chrome, path: str = '/tmp/screenshot.png') -> None:
    driver.find_element(by=By.CSS_SELECTOR, value='body').screenshot(path)  # avoids scrollbar

def main():
    s_path = "."
    if len(sys.argv) >= 2:
        s_path = sys.argv[1]

    print(f"Screenshot save path: {s_path}")

    # Настройки Chrome
    chrome_options = Options()
    chrome_options.add_argument("--start-maximized")  # Максимизировать окно браузера
    chrome_options.add_argument("--disable-infobars")
    chrome_options.add_argument("--disable-extensions")
    chrome_options.add_argument("--headless=new")  # for Chrome >= 109

    # Укажите путь к вашему chromedriver
    service = Service(executable_path='.\\chromedriver-win64\\chromedriver.exe')  # Замените на актуальный путь

    # Инициализация драйвера
    driver = webdriver.Chrome(service=service, options=chrome_options)
    action = ActionChains(driver)

    try:
        # Чтение ссылок из CSV файла
        with open('.\\links.txt', 'r', encoding='utf-8') as file:
            lines = file.readlines()
            progress_s = 0
            progress_d = 100/len(lines)
            for row_number, row in enumerate(lines):
                if not row:  # Пропуск пустых строк
                    continue

                if row[-1] == '\n':
                    row = row[:-1] # Убираем последнюю строку
                if not row.startswith('http'):
                    print(f"Строка {row_number + 1}: Некорректный URL - {row}", file=sys.stdout)
                    sys.stdout.flush()
                    continue

                print(f"Обработка URL: < {row} >", file=sys.stdout)
                print(f"Прогресс: {progress_s}%", file=sys.stdout)
                sys.stdout.flush()
                progress_s += progress_d
                try:
                    # Открытие страницы
                    driver.get(row)
                    fail_counter = 0
                    for i in range(5):
                        time.sleep(10)  # Ожидание загрузки страницы

                        required_width = driver.execute_script(
                            'return document.body.scrollWidth')
                        required_height = driver.execute_script(
                            'return document.body.scrollHeight')
                        driver.set_window_size(1280, required_height)

                        # Поиск элемента для наведения
                        try:
                            element = driver.find_element(
                                By.XPATH,
                                "//a[@data-testid='post_date_block_preview']"
                            )
                            # Наведение мыши
                            action.move_to_element(element).pause(1).perform()
                            print("Наведение на элемент выполнено", file=sys.stdout)
                            sys.stdout.flush()
                            break
                        except NoSuchElementException:
                            print("Элемент для наведения не найден\n", file=sys.stderr)
                            sys.stderr.flush()

                            print("Ожидание 10 сек.", file=sys.stdout)
                            sys.stdout.flush()
                            fail_counter+=1

                    if fail_counter >= 5:
                        raise Exception("Не удалось обработать страницу за 5 попыток. Проверьте подключение к Интернету")

                    views_element = driver.find_element(
                        By.XPATH,
                        "//span[@data-testid='post_date_block_viewers_count']"
                    )
                    views_str = views_element.text.split()[0]
                    views_suffix = views_str[-1]
                    factor = 1
                    views = 0
                    if (views_suffix == "M"):
                        factor = 1000000
                    if (views_suffix == "K"):
                        factor = 1000

                    if factor > 1:
                        views_str = views_str[:-1].replace(",",".")
                        views = float(views_str)*factor
                    else:
                        views = int(views_str)

                    print(f"Просмотры: {views}", file=sys.stdout)
                    sys.stdout.flush()
                    # Создание скриншота
                    timestamp = int(time.time())
                    filename = f"screenshot_{timestamp}_{row_number}.png"
                    save_screenshot(driver, s_path + "/" + filename)
                    print(f"Скриншот сохранен как: {filename}\n")
                    sys.stdout.flush()

                except Exception as e:
                    print(f"Ошибка при обработке URL {row} : \n", file=sys.stderr)
                    print(f"Причина: {str(e)}\n", file=sys.stderr)
                    sys.stderr.flush()
                    continue

    finally:
        driver.quit()
        print("Работа браузера завершена", file=sys.stdout)


if __name__ == "__main__":
    main()