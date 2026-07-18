import gfx.io.GameDelegate;
import gfx.events.EventDispatcher;
import gfx.managers.FocusHandler;
import gfx.ui.InputDetails;
import gfx.ui.NavigationCode;
import Shared.GlobalFunc;
import Components.CrossPlatformButtons;


class RaceSexPanels extends MovieClip
{
	static var bUseClasses:Boolean = true;
	static var bUseTraits:Boolean = true;	

	static var RACE_CATEGORY:Number;
	static var CLASS_CATEGORY:Number;
	static var TRAIT_CATEGORY:Number;
	static var BODY_CATEGORY:Number;
	static var HEAD_CATEGORY:Number;
	static var classFlag:Number;
	static var traitFlag:Number;
	static var UpdateInterval:Number;

	var BackButton:CrossPlatformButtons;
	var DoneButton:CrossPlatformButtons;
	var Mode:Number;
	var NAME_ENTRY:Number;
	var NameEntryInstance:MovieClip;
	var PANEL_ONE:Number;
	var PANEL_TWO_NARROW:Number;
	var PANEL_TWO_WIDE:Number;
	var PanelTwoNarrowInstance:MovieClip;
	var PanelTwoWideInstance:MovieClip;
	var PlayerName:TextField;
	var PlayerRace:TextField;
	var PlayerClass:TextField;
	var PlayerClassLabel:TextField;
	var PlayerTrait:TextField;
	var PlayerTraitLabel:TextField;
	var RaceDescriptionInstance:MovieClip;
	var _CategoriesList:MovieClip;
	var _SubList1:MovieClip;
	var _SubList2:MovieClip;
	var _TextEntryField:MovieClip;
	var bLimitedMenu:Boolean;
	var bShowTextEntry:Boolean;
	var iPlatform:Number;
	var classCallback:String;
	var traitCallback:String;
	var classArray:Array;
	var traitArray:Array;
	var defaultClass:Number;
	var defaultTrait:Number;

	function RaceSexPanels()
	{
		super();

		RACE_CATEGORY = 0;
		if (bUseClasses && bUseTraits) 
		{
			CLASS_CATEGORY = 1;
			TRAIT_CATEGORY = 2;
			BODY_CATEGORY = 3;
			HEAD_CATEGORY = 4;
			classFlag = 1 << 29;
			traitFlag = 1 << 30;

			PlayerClass = _root.RaceSexMenuBaseInstance.BottomBarInstance.PlayerInfo_mc.PlayerClass;
			PlayerClass.textAutoSize = "shrink";
			PlayerTrait = _root.RaceSexMenuBaseInstance.BottomBarInstance.PlayerInfo_mc.PlayerTrait;
			PlayerTrait.textAutoSize = "shrink";
		}
		else if (bUseClasses && !bUseTraits)
		{
			CLASS_CATEGORY = 1;
			BODY_CATEGORY = 2;
			HEAD_CATEGORY = 3;
			classFlag = 1 << 29;

			PlayerClass = _root.RaceSexMenuBaseInstance.BottomBarInstance.PlayerInfo_mc.PlayerClass;
			PlayerClass.textAutoSize = "shrink";
			PlayerTraitLabel = _root.RaceSexMenuBaseInstance.BottomBarInstance.PlayerInfo_mc.APPTRAIT;
			PlayerTraitLabel.text = "";


		}
		else if (bUseTraits && !bUseClasses)
		{
			TRAIT_CATEGORY = 1;
			BODY_CATEGORY = 2;
			HEAD_CATEGORY = 3;
			traitFlag = 1 << 29;

			PlayerClassLabel = _root.RaceSexMenuBaseInstance.BottomBarInstance.PlayerInfo_mc.APPCLASS;
			PlayerClassLabel.text = "";
			PlayerTrait = _root.RaceSexMenuBaseInstance.BottomBarInstance.PlayerInfo_mc.PlayerTrait;
			PlayerTrait.textAutoSize = "shrink";
		}
		else {
			BODY_CATEGORY = 1;
			HEAD_CATEGORY = 2;
		}
		UpdateInterval = -1;

		GlobalFunc.MaintainTextFormat();
		GlobalFunc.SetLockFunction();
		_CategoriesList = _parent.CagetoryLockBaseInstance.CategoryInstance.List_mc;
		_SubList1 = PanelTwoNarrowInstance.List_mc;
		_SubList2 = PanelTwoWideInstance.List_mc;
		_TextEntryField = NameEntryInstance;
		BackButton = _root.RaceSexMenuBaseInstance.BottomBarInstance.ButtonsInstance.BackInstance;
		BackButton._alpha = 0;
		DoneButton = _root.RaceSexMenuBaseInstance.BottomBarInstance.ButtonsInstance.XButtonInstance;
		PlayerName = _root.RaceSexMenuBaseInstance.BottomBarInstance.PlayerInfo_mc.PlayerName;
		PlayerName.textAutoSize = "shrink";
		PlayerRace = _root.RaceSexMenuBaseInstance.BottomBarInstance.PlayerInfo_mc.PlayerRace;
		_TextEntryField._alpha = 0;
		_TextEntryField._x = -2500;
		GameDelegate.addCallBack("SetCategoriesList",this,"SetCategoriesList");
		GameDelegate.addCallBack("ShowTextEntry",this,"ShowTextEntry");
		GameDelegate.addCallBack("SetNameText",this,"SetNameText");
		GameDelegate.addCallBack("SetRaceText",this,"SetRaceText");
		GameDelegate.addCallBack("SetRaceList",this,"SetRaceList");
		GameDelegate.addCallBack("SetOptionSliders",this,"SetSliders");
		GameDelegate.addCallBack("ShowTextEntryField",this,"ShowTextEntryField");
		GameDelegate.addCallBack("moveCategoriesUp",this,"moveCategoriesUp");
		GameDelegate.addCallBack("HideLoadingIcon",this,"HideLoadingIcon");
		GameDelegate.addCallBack("FadeOut",this,"FadeOut");
		EventDispatcher.initialize(this);
		FocusHandler.instance.setFocus(_CategoriesList,0);
		NAME_ENTRY = 0;
		gotoAndStop("1st stop on position");
		PANEL_ONE = _currentframe;
		gotoAndStop("NarrowPanelIn");
		PANEL_TWO_NARROW = _currentframe;
		gotoAndStop("WidePanelIn");
		PANEL_TWO_WIDE = _currentframe;
		Mode = PANEL_TWO_NARROW;
		_root.RaceSexMenuBaseInstance.LoadingIconInstance._visible = false;
	}

	function InitExtensions():Void
	{
		Stage.scaleMode = "showAll";
		_root.RaceSexMenuBaseInstance.RaceSexPanelsInstance.Lock("L");
		_root.RaceSexMenuBaseInstance.BottomBarInstance.Lock("B");
		_root.RaceSexMenuBaseInstance.BottomBarInstance.ButtonsInstance.Lock("L");
		_root.RaceSexMenuBaseInstance.BottomBarInstance.PlayerInfo_mc.Lock("R");
		_root.RaceSexMenuBaseInstance.CagetoryLockBaseInstance.Lock("T");
		this._parent.LeftClickInstance.Lock("T");
      	this._parent.RightClickInstance.Lock("T");
		_TextEntryField.SetupButtons();
		_TextEntryField.TextInputInstance.maxChars = 26;
	}

	static function CallCode(callBack:String, sliderValue:Number, sliderID:Number):Void
	{
		clearInterval(RaceSexPanels.UpdateInterval);
		RaceSexPanels.UpdateInterval = -1;
		GameDelegate.call(callBack,[sliderValue, sliderID]);
	}

	static function PrepForCallCode(callBack:String, sliderValue:Number, sliderID:Number):Void
	{
		clearInterval(RaceSexPanels.UpdateInterval);
		RaceSexPanels.UpdateInterval = -1;
		_root.RaceSexMenuBaseInstance.LoadingIconInstance._visible = true;
		RaceSexPanels.UpdateInterval = setInterval(RaceSexPanels.CallCode, 30, callBack, sliderValue, sliderID);
	}

	function HideLoadingIcon():Void
	{
		_root.RaceSexMenuBaseInstance.LoadingIconInstance._visible = false;
	}

	function SetPlatform(aiPlatform:Number, abPS3Switch:Boolean):Void
	{
		iPlatform = aiPlatform;
		_TextEntryField.SetPlatform(aiPlatform,abPS3Switch);
		_root.RaceSexMenuBaseInstance.BottomBarInstance.ButtonsInstance.XButtonInstance.SetPlatform(aiPlatform,abPS3Switch);
		_root.RaceSexMenuBaseInstance.BottomBarInstance.ButtonsInstance.BackInstance.SetPlatform(aiPlatform,abPS3Switch);
		_root.RaceSexMenuBaseInstance.CagetoryLockBaseInstance.CategoryInstance.ButtonHintRightInstance.SetPlatform(aiPlatform,abPS3Switch);
		_root.RaceSexMenuBaseInstance.CagetoryLockBaseInstance.CategoryInstance.ButtonHintRightInstance.label = "";
		_root.RaceSexMenuBaseInstance.CagetoryLockBaseInstance.CategoryInstance.ButtonHintLeftInstance.SetPlatform(aiPlatform,abPS3Switch);
		_root.RaceSexMenuBaseInstance.CagetoryLockBaseInstance.CategoryInstance.ButtonHintLeftInstance.label = "";
	}

	function ShowTextEntry(abShowTextEntry:Boolean):Void
	{
		bShowTextEntry = abShowTextEntry;
	}

	function SetNameText(astrPlayerName:String):Void
	{
		if (PlayerName != astrPlayerName)
		{
			if (bUseClasses)
			{
				skse.SendModEvent("ClassMenu_Callback",classCallback);
			}
			if (bUseTraits)
			{
				skse.SendModEvent("TraitMenu_Callback",traitCallback);
			}
		}

		PlayerName.SetText(astrPlayerName);
	}

	function SetRaceText(astrPlayerRace:String):Void
	{
		PlayerRace.SetText(astrPlayerRace);
	}

	function SetClassText(astrPlayerClass)
	{
		PlayerClass.SetText(astrPlayerClass);
		classCallback = _SubList1.entryList[_SubList1.selectedIndex].callBack;
	}

	function SetTraitText(astrPlayerTrait)
	{
		PlayerTrait.SetText(astrPlayerTrait);
		traitCallback = _SubList1.entryList[_SubList1.selectedIndex].callBack;
	}

	function IsNarrowPanel(catIndex:Number):Boolean
	{
		if (catIndex == RACE_CATEGORY || (bUseClasses && catIndex == CLASS_CATEGORY) || (bUseTraits && catIndex == TRAIT_CATEGORY))
		{
			return true;
		}
		return false;
	}

	function InitData():Void
	{
		if (bUseClasses)
		{
			classArray = _global.LAM.GetJSONData("Data/SKSE/Plugins/app_classes.json");
			classArray.sortOn("Name",Array.DESCENDING);
			defaultClass = classArray.length - _global.LAM.GetTESGlobal("CLASS");

			PlayerClass.SetText(classArray[defaultClass].Name);
			classCallback = classArray[defaultClass].UniqueKey;
		}
		if (bUseTraits)
		{
			traitArray = _global.LAM.GetJSONData("Data/SKSE/Plugins/app_traits.json");
			traitArray.sortOn("Name",Array.DESCENDING);
			defaultTrait = traitArray.length - _global.LAM.GetTESGlobal("TRAIT");

			PlayerTrait.SetText(traitArray[defaultTrait].Name);
			traitCallback = traitArray[defaultTrait].UniqueKey;
		}
	}

	function onLoad():Void
	{
		_CategoriesList.addEventListener("listMovedUp",this,"onFiltersListMoveUp");
		_CategoriesList.addEventListener("listMovedDown",this,"onFiltersListMoveDown");
		_CategoriesList.addEventListener("itemPress",this,"onItemSelect");
		_parent.LeftClickInstance.addEventListener("click",this,"moveCategoriesDown");
		_parent.RightClickInstance.addEventListener("click",this,"moveCategoriesUp");
		_SubList1.addEventListener("listMovedUp",this,"onNarrowListMoveUp");
		_SubList1.addEventListener("listMovedDown",this,"onNarrowListMoveDown");
		_SubList1.addEventListener("itemPress",this,"onRaceSelect");
		_SubList2.addEventListener("listMovedUp",this,"onWideListMoveUp");
		_SubList2.addEventListener("listMovedDown",this,"onWideListMoveDown");
		_TextEntryField.addEventListener("nameChange",this,"onNameChange");
		DoneButton.addEventListener("click",this,"onDoneClicked");
	}

	function onDoneClicked():Void
	{
		GameDelegate.call("ConfirmDone",[]);
	}

	function SetCategoriesList():Void
	{
		var CAT_TEXT:Number = 0;
		var CAT_FLAG:Number = 1;
		var CAT_STRIDE:Number = 2;
		_CategoriesList.entryList.splice(0,_CategoriesList.entryList.length);

		for (var i:Number = 0; i < arguments.length; i += CAT_STRIDE)
		{
			var entryObject:Object = {text:arguments[i + CAT_TEXT], flag:arguments[i + CAT_FLAG], savedItemIndex:-1};
			_CategoriesList.entryList.push(entryObject);

			if (i == 0)
			{
				if (bUseClasses)
				{
					var classObject:Object = bLimitedMenu ? {text:"", flag:classFlag, savedItemIndex:0} : {text:"$APPCLASSHEADER", flag:classFlag, savedItemIndex:0};
					_CategoriesList.entryList.push(classObject);
				}
				if (bUseTraits)
				{
					var traitObject:Object = bLimitedMenu ? {text:"", flag:traitFlag, savedItemIndex:0} : {text:"$APPTRAITHEADER", flag:traitFlag, savedItemIndex:0};
					_CategoriesList.entryList.push(traitObject);
				}
			}
		}

		if (bLimitedMenu)
		{
			_CategoriesList.selectedIndex = RaceSexPanels.BODY_CATEGORY;
		}
		_CategoriesList.UpdateList();
	}

	function SetRaceList():Void
	{
		var RACE_NAME:Number = 0;
		var RACE_DESCRIPTION:Number = 1;
		var RACE_EQUIPSTATE:Number = 2;
		var RACE_STRIDE:Number = 3;

		_SubList1.entryList.splice(0,_SubList1.entryList.length);

		for (var i:Number = 0; i < arguments.length; i += RACE_STRIDE)
		{
			var entryObject:Object = {text:arguments[i + RACE_NAME], flag:bLimitedMenu ? RaceSexPanels.BODY_CATEGORY : RaceSexPanels.RACE_CATEGORY, raceDescription:arguments[i + RACE_DESCRIPTION].length <= 0 ? "No race description for " + arguments[i + RACE_NAME] : arguments[i + RACE_DESCRIPTION], equipState:arguments[i + RACE_EQUIPSTATE]};
			if (entryObject.equipState > 0)
			{
				_CategoriesList.entryList[entryObject.flag].savedItemIndex = i / RACE_STRIDE;
				SetRaceText(entryObject.text);
			}
			_SubList1.entryList.push(entryObject);
		}
		_SubList1.SaveRaceEntries();
		_SubList1.UpdateList();
		if (bLimitedMenu)
		{
			_CategoriesList.selectedIndex = RaceSexPanels.BODY_CATEGORY;
		}
		else
		{
			_CategoriesList.selectedIndex = RaceSexPanels.RACE_CATEGORY;
		}
		ShowItemsList();
	}

	function SetClassList():Void
	{
		_SubList1.entryList.splice(0,_SubList1.entryList.length);

		for (var entryKey in classArray)
		{
			var classEntry = {text:classArray[entryKey].Name, flag:0, raceDescription:classArray[entryKey].Description, equipState:0, callBack:classArray[entryKey].UniqueKey};
			_SubList1.entryList.push(classEntry);
		}
		classCallback = _SubList1.entryList[0].callBack;
		SetClassText(_SubList1.entryList[0].text);
		_SubList1.SaveClassEntries();
	}

	function SetTraitList():Void
	{
		_SubList1.entryList.splice(0,_SubList1.entryList.length);

		for (var entryKey in traitArray)
		{
			var traitEntry = {text:traitArray[entryKey].Name, flag:0, raceDescription:traitArray[entryKey].Description, equipState:0, callBack:traitArray[entryKey].UniqueKey};
			_SubList1.entryList.push(traitEntry);
		}
		traitCallback = _SubList1.entryList[0].callBack;
		SetTraitText(_SubList1.entryList[0].text);
		_SubList1.SaveTraitEntries();
	}

	function SetSliders():Void
	{
		var SLIDER_NAME = 0;
		var SLIDER_FILTERFLAG = 1;
		var SLIDER_CALLBACKNAME = 2;
		var SLIDER_MIN = 3;
		var SLIDER_MAX = 4;
		var SLIDER_POSITION = 5;
		var SLIDER_INTERVAL = 6;
		var SLIDER_ID = 7;
		var SLIDER_STRIDE = 8;

		_SubList2.entryList.splice(0,_SubList2.entryList.length);

		for (var i:Number = 0; i < arguments.length; i += SLIDER_STRIDE)
		{
			var entryObject:Object = {text:arguments[i + SLIDER_NAME], filterFlag:arguments[i + SLIDER_FILTERFLAG], callbackName:arguments[i + SLIDER_CALLBACKNAME], sliderMin:arguments[i + SLIDER_MIN], sliderMax:arguments[i + SLIDER_MAX], sliderID:arguments[i + SLIDER_ID], position:arguments[i + SLIDER_POSITION], interval:arguments[i + SLIDER_INTERVAL]};
			_SubList2.entryList.push(entryObject);
		}
		_SubList2.UpdateList();
	}

	function handleInput(details:InputDetails, pathToFocus:Array):Boolean
	{
		var handledInput:Boolean = false;
		if (GlobalFunc.IsKeyPressed(details))
		{
			if (details.navEquivalent == NavigationCode.ENTER && Mode == NAME_ENTRY)
			{
				_TextEntryField.onAccept();
			}
			else if (details.navEquivalent == NavigationCode.TAB && Mode == NAME_ENTRY)
			{
				_TextEntryField.onCancel();
				GameDelegate.call("ChangeName",[]);
				handledInput = true;
				if (IsNarrowPanel(_CategoriesList.selectedIndex))
				{
					FocusHandler.instance.setFocus(_SubList1,0);
				}
				else
				{
					FocusHandler.instance.setFocus(_SubList2,0);
				}
			}
			else if (iPlatform != 0)
			{
				if (details.navEquivalent == NavigationCode.GAMEPAD_R2 && RaceSexPanels.UpdateInterval == -1)
				{
					moveCategoriesUp();
				}
				else if (details.navEquivalent == NavigationCode.GAMEPAD_L2 && RaceSexPanels.UpdateInterval == -1)
				{
					moveCategoriesDown();
				}
			}
		}
		if (!handledInput && (RaceSexPanels.UpdateInterval == -1 || _CategoriesList.selectedIndex == RaceSexPanels.RACE_CATEGORY))
		{
			pathToFocus[0].handleInput(details,pathToFocus.slice(1));
		}
		return true;
	}

	function moveCategoriesUp():Void
	{
		if (IsNarrowPanel(_CategoriesList.selectedIndex))
		{
			_CategoriesList.selectedEntry.savedItemIndex = _SubList1.selectedIndex;
		}
		else
		{
			_CategoriesList.selectedEntry.savedItemIndex = _SubList2.selectedIndex;
		}

		_CategoriesList.moveListUp();
	}

	function moveCategoriesDown():Void
	{
		if (IsNarrowPanel(_CategoriesList.selectedIndex))
		{
			_CategoriesList.selectedEntry.savedItemIndex = _SubList1.selectedIndex;
		}
		else
		{
			_CategoriesList.selectedEntry.savedItemIndex = _SubList2.selectedIndex;
		}

		if (bLimitedMenu != true || _CategoriesList.selectedIndex > RaceSexPanels.BODY_CATEGORY)
		{
			_CategoriesList.moveListDown();
		}
	}

	function onFiltersListMoveUp():Void
	{
		GameDelegate.call("PlaySound",["UIMenuFocus"]);
		_CategoriesList._parent.gotoAndPlay("moveLeft");
		if (_CategoriesList.selectedIndex == RaceSexPanels.HEAD_CATEGORY)
		{
			GameDelegate.call("ZoomPC",[true]);
		}
		else if (_CategoriesList.selectedIndex == RaceSexPanels.BODY_CATEGORY)
		{
			GameDelegate.call("ZoomPC",[false]);
		}
		ShowItemsList();
	}

	function onFiltersListMoveDown(event:Object):Void
	{
		GameDelegate.call("PlaySound",["UIMenuFocus"]);
		_CategoriesList._parent.gotoAndPlay("moveRight");
		var zoomInCategory = RaceSexPanels.RACE_CATEGORY;
		if (bUseTraits)
		{
			zoomInCategory = RaceSexPanels.TRAIT_CATEGORY;
		}
		else if (bUseClasses)
		{
			zoomInCategory = RaceSexPanels.CLASS_CATEGORY;
		}
		if (_CategoriesList.selectedIndex == RaceSexPanels.BODY_CATEGORY)
		{
			GameDelegate.call("ZoomPC",[false]);
		}
		else if (_CategoriesList.selectedIndex == zoomInCategory)
		{
			GameDelegate.call("ZoomPC",[true]);
		}
		ShowItemsList();
	}

	function ShowTextEntryField():Void
	{
		if (bShowTextEntry)
		{
			_TextEntryField.TextInputInstance.text = "";
			_TextEntryField.TextInputInstance.focused = true;
			FadeTextEntry(true);
			GameDelegate.call("SetAllowTextInput",[]);
			return;
		}
		GameDelegate.call("ShowVirtualKeyboard",[]);
	}

	function onItemSelect(event:Object):Void
	{
		if (_currentframe == PANEL_ONE)
		{
			ShowItemsList();
		}
	}

	function onNarrowListMoveUp():Void
	{
		if (IsNarrowPanel(_CategoriesList.selectedIndex))
		{
			GameDelegate.call("PlaySound",["UIMenuFocus"]);
			_SubList1._parent.gotoAndPlay("moveUp");
			RaceDescriptionInstance.RaceTextInstance.SetText(_SubList1.entryList[_SubList1.selectedIndex].raceDescription);
			if (RaceSexPanels.UpdateInterval >= 0)
			{
				clearInterval(RaceSexPanels.UpdateInterval);
				RaceSexPanels.UpdateInterval = -1;
				_root.RaceSexMenuBaseInstance.LoadingIconInstance._visible = false;
			}
			switch (_CategoriesList.selectedIndex)
			{
				case RaceSexPanels.RACE_CATEGORY :
					RaceSexPanels.UpdateInterval = setInterval(RaceSexPanels.PrepForCallCode, 600, "ChangeRace", _SubList1.selectedIndex, _SubList1.entryList[_SubList1.selectedIndex].sliderID);
					break;
				case RaceSexPanels.CLASS_CATEGORY :
					SetClassText(_SubList1.entryList[_SubList1.selectedIndex].text);
					break;
				case RaceSexPanels.TRAIT_CATEGORY :
					SetTraitText(_SubList1.entryList[_SubList1.selectedIndex].text);
					break;
			}
		}
	}

	function onNarrowListMoveDown(event:Object):Void
	{
		if (IsNarrowPanel(_CategoriesList.selectedIndex))
		{
			GameDelegate.call("PlaySound",["UIMenuFocus"]);
			_SubList1._parent.gotoAndPlay("moveDown");
			RaceDescriptionInstance.RaceTextInstance.SetText(_SubList1.entryList[_SubList1.selectedIndex].raceDescription);
			if (RaceSexPanels.UpdateInterval >= 0)
			{
				clearInterval(RaceSexPanels.UpdateInterval);
				RaceSexPanels.UpdateInterval = -1;
				_root.RaceSexMenuBaseInstance.LoadingIconInstance._visible = false;
			}
			switch (_CategoriesList.selectedIndex)
			{
				case RaceSexPanels.RACE_CATEGORY :
					RaceSexPanels.UpdateInterval = setInterval(RaceSexPanels.PrepForCallCode, 600, "ChangeRace", _SubList1.selectedIndex, _SubList1.entryList[_SubList1.selectedIndex].sliderID);
					break;
				case RaceSexPanels.CLASS_CATEGORY :
					SetClassText(_SubList1.entryList[_SubList1.selectedIndex].text);
					break;
				case RaceSexPanels.TRAIT_CATEGORY :
					SetTraitText(_SubList1.entryList[_SubList1.selectedIndex].text);
					break;
			}
		}
	}

	function onWideListMoveUp():Void
	{
		GameDelegate.call("PlaySound",["UIMenuFocus"]);
		if (RaceSexPanels.UpdateInterval >= 0)
		{
			_SubList2._parent.gotoAndPlay("moveUp");
		}
	}

	function onWideListMoveDown(event:Object):Void
	{
		GameDelegate.call("PlaySound",["UIMenuFocus"]);
		if (RaceSexPanels.UpdateInterval >= 0)
		{
			_SubList2._parent.gotoAndPlay("moveDown");
		}
	}

	function onNameChange(event:Object):Void
	{
		if (event.nameChanged == true)
		{
			GameDelegate.call("ChangeName",[_TextEntryField.TextInputInstance.text]);
			//if (bUseClasses) {skse.SendModEvent("ClassMenu_Callback",classCallback);}
			//if (bUseTraits) {skse.SendModEvent("TraitMenu_Callback",traitCallback);}
		}
		if (_CategoriesList.selectedIndex == RaceSexPanels.RACE_CATEGORY)
		{
			FocusHandler.instance.setFocus(_SubList1,0);
		}
		else
		{
			FocusHandler.instance.setFocus(_SubList2,0);
		}
		FadeTextEntry(false);
	}

	function FadeTextEntry(bIn:Boolean):Void
	{
		Mode = bIn ? NAME_ENTRY : PANEL_ONE;
		if (bIn)
		{
			_TextEntryField._x = 500;
		}
		_TextEntryField.onEnterFrame = function()
		{
			_alpha = _alpha + (bIn ? 10 : -10);
			if (bIn ? _alpha >= 100 : _alpha <= 0)
			{
				if (!bIn)
				{
					if (_TextEntryField.TextInputInstance.text == undefined)
					{
						GameDelegate.call("ChangeName",[]);
					}
					else
					{
						GameDelegate.call("ChangeName",[_TextEntryField.TextInputInstance.text]);
					}
				}
				delete onEnterFrame;
				if (bIn)
				{
					return;
				}
				_TextEntryField.TextInputInstance._x = -2500;
			}
		};
	}

	function ShowItemsList():Void
	{
		switch (_CategoriesList.selectedIndex)
		{
			case RaceSexPanels.RACE_CATEGORY :
				gotoAndPlay("narrowPanel2Show");
				FadeTextEntry(false);
				FocusHandler.instance.setFocus(_SubList1,0);
				_SubList1.selectedIndex = _CategoriesList.selectedEntry.savedItemIndex;
				_SubList1.SwapToRaces();
				_SubList1.UpdateList();
				RaceDescriptionInstance.RaceTextInstance.SetText(_SubList1.entryList[_SubList1.selectedIndex].raceDescription);
				break;
			case RaceSexPanels.CLASS_CATEGORY :
				if (bUseClasses)
				{
					gotoAndPlay("narrowPanel2Show");
					FadeTextEntry(false);
					FocusHandler.instance.setFocus(_SubList1,0);
					_SubList1.selectedIndex = _CategoriesList.selectedEntry.savedItemIndex;
					_SubList1.SwapToClasses();
					if (_SubList1.entryList.length == 0)
					{
						SetClassList();
					}
					_SubList1.UpdateList();
					RaceDescriptionInstance.RaceTextInstance.SetText(_SubList1.entryList[_SubList1.selectedIndex].raceDescription);
				}
				break;
			case RaceSexPanels.TRAIT_CATEGORY :
				if (bUseTraits)
				{
					gotoAndPlay("narrowPanel2Show");
					FadeTextEntry(false);
					FocusHandler.instance.setFocus(_SubList1,0);
					_SubList1.selectedIndex = _CategoriesList.selectedEntry.savedItemIndex;
					_SubList1.SwapToTraits();
					if (_SubList1.entryList.length == 0)
					{
						SetTraitList();
					}
					_SubList1.UpdateList();
					RaceDescriptionInstance.RaceTextInstance.SetText(_SubList1.entryList[_SubList1.selectedIndex].raceDescription);
				}
				break;
			default :
				gotoAndPlay("widePanel2Show");
				FadeTextEntry(false);
				FocusHandler.instance.setFocus(_SubList2,0);
				_SubList2.itemFilter = _CategoriesList.selectedEntry.flag;
				_SubList2.selectedIndex = _CategoriesList.selectedEntry.savedItemIndex;
				_SubList2.UpdateList();

		}

		GameDelegate.call("PlaySound",["UIMenuBladeOpenSD"]);
	}

	function HideItemsList():Void
	{
		if (IsNarrowPanel(_CategoriesList.selectedIndex))
		{
			gotoAndPlay("narrowPanel2Hide");
			FocusHandler.instance.setFocus(_CategoriesList,0);
			_CategoriesList.selectedEntry.savedItemIndex = _SubList1.selectedIndex;
		}
		else
		{
			gotoAndPlay("widePanel2Hide");
			FocusHandler.instance.setFocus(_CategoriesList,0);
			_CategoriesList.selectedEntry.savedItemIndex = _SubList2.selectedIndex;
			BackButton._alpha = 0;
		}

		GameDelegate.call("PlaySound",["UIMenuBladeCloseSD"]);
	}

	function FadeOut():Void
	{
		_root.gotoAndPlay(2);
	}

}