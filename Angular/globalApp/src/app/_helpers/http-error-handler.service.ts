import { Injectable } from '@angular/core';
import { HttpErrorResponse } from '@angular/common/http';

import { Observable, of } from 'rxjs';

import { MessageService } from './message.service';
import { PopupInformativoComponent, PopupInformativoModel } from '../modales/popup-informativo/popup-informativo.component';
import { MatDialog } from '@angular/material';
import { Router } from '@angular/router';

/* Type of the handleError function returned by HttpErrorHandler.createHandleError */
export type handleError = <T> (operation?: string, result?: T) => (error: HttpErrorResponse) => Observable<T>;

/* handles HttpClient errors */

@Injectable()
export class HttpErrorHandlerService {

  constructor(private messageService: MessageService, public dialog: MatDialog, private router: Router) { }

  /* Create curried handleError function that already knows the service name */
  createHandleError = (serviceName = '') => <T>
    (operation = 'operation', result = {} as T) => this.handleError(serviceName, operation, result)// ;

  /**
   * Returns a function that handles Http operation failures.
   * This error handler lets the app continue to run as if no error ocurred.
   * @param serviceName = name of the data service that attempted the operation
   * @param operation - name of the operation failed
   * @param result - optional value to return as the observable result
   */
  handleError<T>(serviceName = '', operation = 'operation', result = {} as T) {

    return (error: HttpErrorResponse): Observable<T> => {
      // TODO: send the error to remote logging infraestructure
      console.error(error); // log to console instead

      const message = (error.error instanceof ErrorEvent) ?
        error.error.message :
        `server returned code ${error.status} with body "${error.error}"`;

      // TODO: better job of transforming error for user consumption
      this.messageService.add(`${serviceName}: ${operation} failed: ${message}`);

      console.log(`${serviceName}: ${operation} failed: ${message}`);

      // window.alert(`${serviceName}: ${operation} failed: ${message}`);

      const message2 = (error.error instanceof ErrorEvent) ?
        error.error.message :
        `${serviceName}: ${operation} <br> Código ${error.status}: "${error.error}"`;

      const dialogData = new PopupInformativoModel('Error', message2, 'danger');
      const dialogRef = this.dialog.open(PopupInformativoComponent, {data: dialogData});

      // TODO: Desloguear si son errores pertenecientes a un problema de token
      if (error.status === 401 || error.status === 408 || error.status === 417) {
        this.router.navigate(['/logout']);
      }

      // Let the app keep running by returning a safe result.
      return of( result );
    };
  }
}
