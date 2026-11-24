import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { FormsModule } from '@angular/forms';

import { ReportesListaComponent } from './reportes-lista/reportes-lista.component';
import { ReporteDetalleComponent } from './reporte-detalle/reporte-detalle.component';
import { ReportesComponent } from './reportes/reportes.component';
import { ReportesHomeComponent } from './reportes-home/reportes-home.component';

import { ReportesRoutingModule } from './reportes-routing.module';

@NgModule({
  declarations: [
    ReportesComponent,
    ReportesListaComponent,
    ReportesHomeComponent,
    ReporteDetalleComponent
  ],
  imports: [
    CommonModule,
    FormsModule,
    ReportesRoutingModule
  ]
})
export class ReportesModule { }
