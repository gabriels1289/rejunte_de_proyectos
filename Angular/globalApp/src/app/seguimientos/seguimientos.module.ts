import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { SeguimientosRoutingModule } from './seguimientos-routing.module';
import { SeguimientosComponent } from './seguimientos/seguimientos.component';
import { SharedModule } from '../utilidades/shared/shared.module';
import { NuevoSeguimientoComponent } from '../empleados/modales/nuevo-seguimiento/nuevo-seguimiento.component';


@NgModule({
  declarations: [SeguimientosComponent],
  imports: [
    CommonModule,
    SeguimientosRoutingModule,
    SharedModule
  ],
  entryComponents: [NuevoSeguimientoComponent]
})
export class SeguimientosModule { }
