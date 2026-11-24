import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { AdminRoutingModule } from './admin-routing.module';

import { AdminDashboardComponent } from './admin-dashboard/admin-dashboard.component';
import { AdminComponent } from './admin/admin.component';
import { GestionReportesComponent } from './gestion-reportes/gestion-reportes.component';
import { GestionEmpleadosComponent } from './gestion-empleados/gestion-empleados.component';


@NgModule({
  declarations: [
    AdminDashboardComponent,
    AdminComponent,
    GestionReportesComponent,
    GestionEmpleadosComponent
  ],
  imports: [
    CommonModule,
    AdminRoutingModule
  ]
})
export class AdminModule { }
