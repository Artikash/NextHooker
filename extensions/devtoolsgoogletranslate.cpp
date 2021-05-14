﻿#include "qtcommon.h"
#include "devtools.h"
#include <QFileDialog>
#include <QMouseEvent>
#include <ShlObj.h>

extern const wchar_t* TRANSLATION_ERROR;
extern const char* CHROME_LOCATION;
extern const char* START_DEVTOOLS;
extern const char* STOP_DEVTOOLS;
extern const char* HEADLESS_MODE;
extern const char* DEVTOOLS_STATUS;
extern const char* AUTO_START;
extern const wchar_t* ERROR_START_CHROME;

extern Synchronized<std::wstring> translateTo, translateFrom;
extern QFormLayout* display;
extern Settings settings;

const char* TRANSLATION_PROVIDER = "DevTools Google Translate";
const char* GET_API_KEY_FROM = nullptr;
bool translateSelectedOnly = true, rateLimitAll = false, rateLimitSelected = false, useCache = true, useFilter = true;
bool firstTranslation = true;
int tokenCount = 30, tokenRestoreDelay = 60000, maxSentenceSize = 2500;

QStringList languages
{
	"Afrikaans: af",
	"Albanian: sq",
	"Amharic: am",
	"Arabic: ar",
	"Armenian: hy",
	"Azerbaijani: az",
	"Basque: eu",
	"Belarusian: be",
	"Bengali: bn",
	"Bosnian: bs",
	"Bulgarian: bg",
	"Catalan: ca",
	"Cebuano: ceb",
	"Chichewa: ny",
	"Chinese (simplified): zh",
	"Chinese (traditional): zh-TW",
	"Corsican: co",
	"Croatian: hr",
	"Czech: cs",
	"Danish: da",
	"Dutch: nl",
	"English: en",
	"Esperanto: eo",
	"Estonian: et",
	"Filipino: tl",
	"Finnish: fi",
	"French: fr",
	"Frisian: fy",
	"Galician: gl",
	"Georgian: ka",
	"German: de",
	"Greek: el",
	"Gujarati: gu",
	"Haitian Creole: ht",
	"Hausa: ha",
	"Hawaiian: haw",
	"Hebrew: iw",
	"Hindi: hi",
	"Hmong: hmn",
	"Hungarian: hu",
	"Icelandic: is",
	"Igbo: ig",
	"Indonesian: id",
	"Irish: ga",
	"Italian: it",
	"Japanese: ja",
	"Javanese: jw",
	"Kannada: kn",
	"Kazakh: kk",
	"Khmer: km",
	"Kinyarwanda: rw",
	"Korean: ko",
	"Kurdish (Kurmanji): ku",
	"Kyrgyz: ky",
	"Lao: lo",
	"Latin: la",
	"Latvian: lv",
	"Lithuanian: lt",
	"Luxembourgish: lb",
	"Macedonian: mk",
	"Malagasy: mg",
	"Malay: ms",
	"Malayalam: ml",
	"Maltese: mt",
	"Maori: mi",
	"Marathi: mr",
	"Mongolian: mn",
	"Myanmar (Burmese): my",
	"Nepali: ne",
	"Norwegian: no",
	"Odia (Oriya): or",
	"Pashto: ps",
	"Persian: fa",
	"Polish: pl",
	"Portuguese: pt",
	"Punjabi: pa",
	"Romanian: ro",
	"Russian: ru",
	"Samoan: sm",
	"Scots Gaelic: gd",
	"Serbian: sr",
	"Sesotho: st",
	"Shona: sn",
	"Sindhi: sd",
	"Sinhala: si",
	"Slovak: sk",
	"Slovenian: sl",
	"Somali: so",
	"Spanish: es",
	"Sundanese: su",
	"Swahili: sw",
	"Swedish: sv",
	"Tajik: tg",
	"Tamil: ta",
	"Tatar: tt",
	"Telugu: te",
	"Thai: th",
	"Turkish: tr",
	"Turkmen: tk",
	"Ukrainian: uk",
	"Urdu: ur",
	"Uyghur: ug",
	"Uzbek: uz",
	"Vietnamese: vi",
	"Welsh: cy",
	"Xhosa: xh",
	"Yiddish: yi",
	"Yoruba: yo",
	"Zulu: zu"
};
std::wstring autoDetectLanguage = L"auto";

BOOL WINAPI DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		QString chromePath = settings.value(CHROME_LOCATION).toString();
		wchar_t programFiles[MAX_PATH + 100] = {};
		if (chromePath.isEmpty()) for (auto folder : { CSIDL_PROGRAM_FILESX86, CSIDL_PROGRAM_FILES, CSIDL_LOCAL_APPDATA })
		{
			SHGetFolderPathW(NULL, folder, NULL, SHGFP_TYPE_CURRENT, programFiles);
			wcscat_s(programFiles, L"/Google/Chrome/Application/chrome.exe");
			if (std::filesystem::exists(programFiles)) chromePath = S(programFiles);
		}
		auto chromePathEdit = new QLineEdit(chromePath);
		static struct : QObject
		{
			bool eventFilter(QObject* object, QEvent* event)
			{
				if (auto mouseEvent = dynamic_cast<QMouseEvent*>(event))
					if (mouseEvent->button() == Qt::LeftButton)
						if (QString chromePath = QFileDialog::getOpenFileName(nullptr, TRANSLATION_PROVIDER, "/", "Chrome (*.exe)"); !chromePath.isEmpty())
							((QLineEdit*)object)->setText(chromePath);
				return false;
			}
		} chromeSelector;
		chromePathEdit->installEventFilter(&chromeSelector);
		QObject::connect(chromePathEdit, &QLineEdit::textChanged, [chromePathEdit](QString path) { settings.setValue(CHROME_LOCATION, path); });
		display->addRow(CHROME_LOCATION, chromePathEdit);
		auto statusLabel = new QLabel("Stopped");
		auto startButton = new QPushButton(START_DEVTOOLS), stopButton = new QPushButton(STOP_DEVTOOLS);
		auto headlessCheck = new QCheckBox();
		headlessCheck->setChecked(settings.value(HEADLESS_MODE, true).toBool());
		QObject::connect(headlessCheck, &QCheckBox::clicked, [](bool headless) { settings.setValue(HEADLESS_MODE, headless); });
		QObject::connect(startButton, &QPushButton::clicked, [statusLabel, chromePathEdit, headlessCheck]
		{
			DevTools::Start(
				S(chromePathEdit->text()),
				[statusLabel](QString status)
				{
					QMetaObject::invokeMethod(statusLabel, std::bind(&QLabel::setText, statusLabel, status));
					if (status == "ConnectedState") std::thread([]
					{
						if (HttpRequest httpRequest{
							L"Mozilla/5.0 Textractor",
							L"127.0.0.1",
							L"POST",
							L"/json/version",
							"",
							NULL,
							9222,
							NULL,
							WINHTTP_FLAG_ESCAPE_DISABLE
						})
							if (auto userAgent = Copy(JSON::Parse(httpRequest.response)[L"User-Agent"].String()))
								if (userAgent->find(L"Headless") != std::string::npos)
									DevTools::SendRequest(
										"Network.setUserAgentOverride",
										FormatString(LR"({"userAgent":"%s"})", userAgent->replace(userAgent->find(L"Headless"), 8, L""))
									);
					}).detach();
				},
				headlessCheck->isChecked()
			);
		});
		QObject::connect(stopButton, &QPushButton::clicked, &DevTools::Close);
		auto buttons = new QHBoxLayout();
		buttons->addWidget(startButton);
		buttons->addWidget(stopButton);
		display->addRow(HEADLESS_MODE, headlessCheck);
		auto autoStartCheck = new QCheckBox();
		autoStartCheck->setChecked(settings.value(AUTO_START, false).toBool());
		QObject::connect(autoStartCheck, &QCheckBox::clicked, [](bool autoStart) { settings.setValue(AUTO_START, autoStart); });
		display->addRow(AUTO_START, autoStartCheck);
		display->addRow(buttons);
		statusLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
		display->addRow(DEVTOOLS_STATUS, statusLabel);
		if (autoStartCheck->isChecked()) QMetaObject::invokeMethod(startButton, &QPushButton::click, Qt::QueuedConnection);
	}
	break;
	case DLL_PROCESS_DETACH:
	{
		DevTools::Close();
	}
	break;
	}
	return TRUE;
}

std::pair<bool, std::wstring> Translate(const std::wstring& text)
{
	if (!DevTools::Connected()) return { false, FormatString(L"%s: %s", TRANSLATION_ERROR, ERROR_START_CHROME) };
	// DevTools can't handle concurrent translations yet
	static std::mutex translationMutex;
	std::scoped_lock lock(translationMutex);
	if (firstTranslation) {
		firstTranslation = false; 
		DevTools::SendRequest("Page.navigate", FormatString(LR"({"url":"https://translate.google.com"})"));
		for (int retry = 0; ++retry < 100; Sleep(100))
			if (auto translation = Copy(DevTools::SendRequest("Runtime.evaluate",
				LR"({"expression":"document.querySelector('#yDmH0d [class=Qhg5gf]').innerHTML.trim() ","returnByValue":true})"
			)[L"result"][L"value"].String())) if (!translation->empty()) break;

		DevTools::SendRequest("Runtime.evaluate", FormatString(LR"({"expression":"
			document.querySelector('#yDmH0d').querySelector('button').click();
		"})"));
		Sleep(2000);
	}

	DevTools::SendRequest("Page.navigate", FormatString(LR"({"url":"https://translate.google.com/?op=translate&sl=%s&tl=%s&text=%s"})", translateFrom.Copy(), translateTo.Copy(), Escape(text)));

	std::wstring pattern = translateFrom.Copy() + L"-" + translateTo.Copy() + L"-";
	for (int retry = 0; ++retry < 100; Sleep(100))
		if (auto translation = Copy(DevTools::SendRequest("Runtime.evaluate",
			LR"({"expression":"document.querySelector('#yDmH0d').querySelector('.VIiyi > span > span').__incrementalDOMData.key.trim() ","returnByValue":true})"
		)[L"result"][L"value"].String())) if (!translation->empty()) return { true, translation.value().erase(0, translation.value().find(pattern) + pattern.length()) };

	if (auto errorMessage = Copy(DevTools::SendRequest("Runtime.evaluate",
		LR"({"expression":"document.querySelector('div.lmt__system_notification').innerHTML","returnByValue":true})"
	)[L"result"][L"value"].String())) return { false, FormatString(L"%s: %s", TRANSLATION_ERROR, errorMessage.value()) };
	return { false, TRANSLATION_ERROR };
}
