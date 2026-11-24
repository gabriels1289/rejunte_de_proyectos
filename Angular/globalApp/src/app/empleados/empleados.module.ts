import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';

import { EmpleadoDetalleComponent } from './empleado-detalle/empleado-detalle.component';

import { EmpleadosRoutingModule } from './empleados-routing.module';
import { EmpleadosComponent } from './empleados/empleados.component';
import { ListadoComponent } from './listado/listado.component';
import { SharedModule } from '../utilidades/shared/shared.module';
import { DatosPersonalesComponent } from './datos-personales/datos-personales.component';
import { SeguimientosComponent } from './seguimientos/seguimientos.component';
import { EvaluacionesComponent } from './evaluaciones/evaluaciones.component';
import { NuevoSeguimientoComponent } from './modales/nuevo-seguimiento/nuevo-seguimiento.component';
import { FormularioComponent } from './formulario/formulario.component';

@NgModule({
  declarations: [
    EmpleadoDetalleComponent,
    EmpleadosComponent,
    ListadoComponent,
    DatosPersonalesComponent,
    SeguimientosComponent,
    EvaluacionesComponent,
    NuevoSeguimientoComponent,
    FormularioComponent
  ],
  imports: [
    CommonModule,
    FormsModule,
    EmpleadosRoutingModule,
    SharedModule
  ],
  entryComponents: [NuevoSeguimientoComponent]
})
export class EmpleadosModule { }
