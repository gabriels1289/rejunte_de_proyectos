import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { MisAjustesRoutingModule } from './mis-ajustes-routing.module';
import { FormsModule } from '@angular/forms';
import { SharedModule } from '../utilidades/shared/shared.module';
import { PerfilComponent } from './perfil/perfil.component';
import { LogsComponent } from './logs/logs.component';
import { CambiarPasswordComponent } from './cambiar-password/cambiar-password.component';


@NgModule({
  declarations: [
    PerfilComponent,
    LogsComponent,
    CambiarPasswordComponent
  ],
  imports: [
    CommonModule,
    MisAjustesRoutingModule,
    SharedModule,
    FormsModule
  ],
  entryComponents: [
    CambiarPasswordComponent
  ]
})
export class MisAjustesModule { }
