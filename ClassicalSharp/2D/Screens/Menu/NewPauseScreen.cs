﻿using System;
using System.Drawing;
using OpenTK.Input;

namespace ClassicalSharp {
	
	public class NewPauseScreen : MenuScreen {
		
		public NewPauseScreen( Game game ) : base( game ) {
		}		
		
		public override void Init() {
			titleFont = new Font( "Arial", 16, FontStyle.Bold );
			buttons = new ButtonWidget[] {
				Make( 0, -50, "Options", Docking.Centre, g => g.SetNewScreen( new OptionsScreen( g ) ) ),
				Make( 0, 0, "Environment settings", Docking.Centre, g => g.SetNewScreen( new EnvSettingsScreen( g ) ) ),
				Make( 0, 50, "Key mappings", Docking.Centre, g => g.SetNewScreen( new KeyMappingsScreen( g ) ) ),
				Make( 0, 55, "Back to game", Docking.BottomOrRight, g => g.SetNewScreen( new NormalScreen( g ) ) ),
				Make( 0, 5, "Exit", Docking.BottomOrRight, g => g.Exit() ),
			};
		}
		
		ButtonWidget Make( int x, int y, string text, Docking vDocking, Action<Game> onClick ) {
			return ButtonWidget.Create( game, x, y, 240, 35, text, Docking.Centre, vDocking, titleFont, onClick );
		}
	}
}