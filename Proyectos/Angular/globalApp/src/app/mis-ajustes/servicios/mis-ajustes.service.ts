import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { catchError } from 'rxjs/operators';

// esto es para intentar acceder a los empleados mediante la base de datos
import { HttpClient, HttpParams, HttpHeaders, HttpResponse } from '@angular/common/http';
import { HttpErrorHandlerService, handleError } from '../../_helpers/http-error-handler.service';

// importo la direccion de la api
import { environment } from '../../../environments/environment';  // TODO: cambiar a prod

@Injectable({
  providedIn: 'root'
})
export class MisAjustesService {

  private handleError: handleError;

  constructor(
  private http: HttpClient,
  httpErrorhandler: HttpErrorHandlerService
  ) {
    this.handleError = httpErrorhandler.createHandleError('AjustesService');
  }

  putPassword(data: any) {
    return this.http.put(`${environment.apiUrl}/index.php/ajustes/perfil`, data, {observe: 'response'})
      .pipe(
        catchError(this.handleError('putPassword', []))
      );
  }

  getLogs(cantidad: number) {
    const  params = new  HttpParams().set('cantidad', cantidad.toString());
    return this.http.get(`${environment.apiUrl}/index.php/ajustes/logs`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getLogs', []))
      );
  }

}
